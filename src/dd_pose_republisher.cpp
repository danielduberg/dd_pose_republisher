#include <dd_pose_republisher/dd_pose_republisher.h>

#include <QLabel>

namespace dd_pose_republisher
{

DDPoseRepublisherPanel::DDPoseRepublisherPanel(QWidget* parent)
    : rviz::Panel(parent)
{
  in_topic_menu_ = new DDQComboBox;

  out_topic_editor_ = new QLineEdit;

  frequency_ = new QDoubleSpinBox;
  frequency_->setMinimum(1);
  frequency_->setMaximum(std::numeric_limits<double>::infinity());
  frequency_->setSingleStep(5);
  frequency_->setValue(10);

  publish_timer_ = nh_.createTimer(ros::Rate(frequency_->value()),
                                   &DDPoseRepublisherPanel::posePublish, this);

  start_publishing_ = new QPushButton;
  start_publishing_->setText("Start");
  start_publishing_->setStyleSheet(
      QString("color: %1").arg(QColor(Qt::green).name()));

  stop_publishing_ = new QPushButton;
  stop_publishing_->setText("Stop");
  stop_publishing_->setStyleSheet(
      QString("color: %1").arg(QColor(Qt::red).name()));

  layout = new QGridLayout();
  layout->addWidget(new QLabel("Input Topic:"), 0, 0, 1, 2);
  layout->addWidget(in_topic_menu_, 0, 2, 1, 2);
  layout->addWidget(new QLabel("Output Topic:"), 1, 0, 1, 2);
  layout->addWidget(out_topic_editor_, 1, 2, 1, 2);
  layout->addWidget(new QLabel("Frequency:"), 2, 0);
  layout->addWidget(frequency_, 2, 2, 1, 2);
  layout->addWidget(stop_publishing_, 3, 1);
  layout->addWidget(start_publishing_, 3, 2);
  setLayout(layout);

  // Next we make signal/slot connections.
  connect(in_topic_menu_, SIGNAL(activated(int)), this, SLOT(updateTopic()));
  connect(out_topic_editor_, SIGNAL(editingFinished()), this,
          SLOT(updateTopic()));
  connect(frequency_, SIGNAL(valueChanged(double)), this,
          SLOT(updateFrequency()));
  connect(start_publishing_, SIGNAL(released()), this, SLOT(startPublishing()));
  connect(stop_publishing_, SIGNAL(released()), this, SLOT(stopPublishing()));
}

void DDPoseRepublisherPanel::load(const rviz::Config& config)
{
  rviz::Panel::load(config);
  QString topic_in;
  if (config.mapGetString("in_topic", &topic_in))
  {
    in_topic_menu_->clear();
    in_topic_menu_->insertItem(0, topic_in);
    in_topic_menu_->setCurrentIndex(0);
    in_topic_menu_->setCurrentText(topic_in);
    updateTopic();
  }

  QString topic_out;
  if (config.mapGetString("out_topic", &topic_out))
  {
    out_topic_editor_->setText(topic_out);
    updateTopic();
  }

  float frequency;
  if (config.mapGetFloat("frequency", &frequency))
  {
    frequency_->setValue(frequency);
    updateFrequency();
  }
}

void DDPoseRepublisherPanel::save(rviz::Config config) const
{
  rviz::Panel::save(config);
  config.mapSetValue("in_topic", in_topic_);
  config.mapSetValue("out_topic", out_topic_);
  config.mapSetValue("frequency", frequency_->value());
}

void DDPoseRepublisherPanel::updateTopic()
{
  setTopic(in_topic_menu_->itemText(in_topic_menu_->currentIndex()),
           out_topic_editor_->text());
}

void DDPoseRepublisherPanel::setTopic(const QString& new_in_topic,
                                      const QString& new_out_topic)
{
  QString new_out_topic_temp = new_out_topic;
  if (new_out_topic_temp[0] != QChar('/'))
  {
    new_out_topic_temp.prepend('/');
    out_topic_editor_->setText(new_out_topic_temp);
  }
  if (new_in_topic != in_topic_ || new_out_topic_temp != out_topic_)
  {
    in_topic_ = new_in_topic;
    out_topic_ = new_out_topic_temp;
    if (out_topic_ == "" || in_topic_ == "")
    {
      pose_sub_.shutdown();
      pose_pub_.shutdown();
    }
    else if (out_topic_ == in_topic_)
    {
      ROS_WARN("Nav Goal: The two topics shall not be the same!");
      out_topic_editor_->setText("The two topics shall not be the same!");
      pose_sub_.shutdown();
      pose_pub_.shutdown();
    }
    else
    {
      pose_sub_ = nh_.subscribe<geometry_msgs::PoseStamped>(
          in_topic_.toStdString(), 1, &DDPoseRepublisherPanel::poseCallback,
          this);
      try
      {
        pose_pub_ = nh_.advertise<geometry_msgs::PoseStamped>(
            out_topic_.toStdString(), 1);
      }
      catch (ros::InvalidNameException e)
      {
        ROS_WARN("Nav Goal: %s", e.what());
      }
    }
    // rviz::Panel defines the configChanged() signal.  Emitting it
    // tells RViz that something in this panel has changed that will
    // affect a saved config file.  Ultimately this signal can cause
    // QWidget::setWindowModified(true) to be called on the top-level
    // rviz::VisualizationFrame, which causes a little asterisk ("*")
    // to show in the window's title bar indicating unsaved changes.
    Q_EMIT configChanged();
  }

  // TODO: Disable slider?
}

void DDPoseRepublisherPanel::startPublishing() { publish_timer_.start(); }

void DDPoseRepublisherPanel::stopPublishing() { publish_timer_.stop(); }

void DDPoseRepublisherPanel::updateFrequency()
{
  publish_timer_.setPeriod(ros::Duration(1.0 / frequency_->value()), false);
}

void DDPoseRepublisherPanel::poseCallback(
    const geometry_msgs::PoseStamped::ConstPtr& pose)
{
  pose_ = *pose;
}

void DDPoseRepublisherPanel::posePublish(const ros::TimerEvent& event)
{
  pose_.header.stamp = ros::Time::now();
  pose_pub_.publish(pose_);
}
}

// Tell pluginlib about this class.  Every class which should be
// loadable by pluginlib::ClassLoader must have these two lines
// compiled in its .cpp file, outside of any namespace scope.
#include <pluginlib/class_list_macros.h>
PLUGINLIB_EXPORT_CLASS(dd_pose_republisher::DDPoseRepublisherPanel, rviz::Panel)
