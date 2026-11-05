#ifndef NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_RECVSCREEN_H_
#define NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_RECVSCREEN_H_

#include <QWidget>

class T_RecvScreen : public QWidget
{
  Q_OBJECT

public:
  explicit T_RecvScreen(QWidget *parent = nullptr);
  ~T_RecvScreen();

  Q_SLOT void onSendHandleResult(QPixmap const &pix);

protected:
  void paintEvent(QPaintEvent *event) override;

private:
  QPixmap _pix;
};

#endif //NXFRAMEWORK_NEXUSEXAMPLE_EXAMPLEPAGE_T_RECVSCREEN_H_
