#ifndef DD_POSE_REPUBLISHER_DD_Q_COMBO_BOX_H
#define DD_POSE_REPUBLISHER_DD_Q_COMBO_BOX_H

#ifndef Q_MOC_RUN
#include <ros/ros.h>
#endif

#include <QComboBox>

namespace dd_pose_republisher
{
class DDQComboBox : public QComboBox
{
  Q_OBJECT
public:
  explicit DDQComboBox(QWidget* parent = 0);
  virtual ~DDQComboBox();

public:
  virtual void showPopup();
};
}

#endif // DD_POSE_REPUBLISHER_DD_Q_COMBO_BOX_H
