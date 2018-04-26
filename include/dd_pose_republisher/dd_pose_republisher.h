#ifndef DD_POSE_REPUBLISHER_PANEL_H
#define DD_POSE_REPUBLISHER_PANEL_H

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#include <rviz/panel.h>
#endif

#include <QGridLayout>
#include <QDoubleSpinBox>
#include <QComboBox>
#include <QPushButton>
#include <QLineEdit>

#include <geometry_msgs/PoseStamped.h>

#include <dd_pose_republisher/dd_q_combo_box.h>

namespace dd_pose_republisher
{
class DDPoseRepublisherPanel : public rviz::Panel
{
  Q_OBJECT
public:
  DDPoseRepublisherPanel(QWidget* parent = 0);

  virtual void load(const rviz::Config& config);
  virtual void save(rviz::Config config) const;

public Q_SLOTS:

protected Q_SLOTS:

  void updateTopic();

  void setTopic(const QString& new_in_topic, const QString& new_out_topic);

  void startPublishing();

  void stopPublishing();

  void updateFrequency();

  void poseCallback(const geometry_msgs::PoseStamped::ConstPtr& pose);

  void posePublish(const ros::TimerEvent& event);

protected:
  QGridLayout* layout;

  DDQComboBox* in_topic_menu_;
  // One-line text editor for entering the outgoing ROS topic name.
  QLineEdit* out_topic_editor_;

  // The current name of the output topic.
  QString in_topic_;
  QString out_topic_;

  QDoubleSpinBox* frequency_;
  QPushButton* start_publishing_;
  QPushButton* stop_publishing_;

  // The ROS node handle.
  ros::NodeHandle nh_;

  geometry_msgs::PoseStamped pose_;

  ros::Timer publish_timer_;

  ros::Subscriber pose_sub_;

  ros::Publisher pose_pub_;
};
}

#endif // DD_POSE_REPUBLISHER_PANEL_H
