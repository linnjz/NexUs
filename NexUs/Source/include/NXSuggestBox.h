#ifndef NXSUGGESTBOX_H
#define NXSUGGESTBOX_H

#include <QVariantMap>
#include <QWidget>

#include "NXDef.h"

class NXSuggestBoxPrivate;

class NX_EXPORT NXSuggestBox : public QWidget
{
  Q_OBJECT
  Q_Q_CREATE(NXSuggestBox)
  Q_PROPERTY_CREATE_H(int, BorderRadius)
  Q_PROPERTY_CREATE_H(Qt::CaseSensitivity, CaseSensitivity)

public:
  explicit NXSuggestBox(QWidget *parent = nullptr);
  ~NXSuggestBox();
  void setPlaceholderText(QString const &placeholderText);
  void setFixedSize(QSize size);
  void setFixedSize(int w, int h);
  void setFixedHeight(int h);

  struct NX_EXPORT SuggestData
  {
    Q_PRIVATE_CREATE(QPS_SCREF(QString), SuggestText)
    Q_PRIVATE_CREATE(QPS_SCREF(QString), SuggestKey)
    Q_PRIVATE_CREATE(QPS_SCREF(QVariantMap), SuggestData)
    Q_PRIVATE_CREATE(NXIconType::IconName, NXIcon)

  public:
    explicit SuggestData();
    explicit SuggestData(NXIconType::IconName icon, QString const &suggestText, QVariantMap const &suggestData = {});
    ~SuggestData();
  };

  QString addSuggestion(QString const &suggestText, QVariantMap const &suggestData = {});
  QString addSuggestion(NXIconType::IconName icon, QString const &suggestText, QVariantMap const &suggestData = {});
  QString addSuggestion(NXSuggestBox::SuggestData const &suggestData);
  QStringList addSuggestion(QList<NXSuggestBox::SuggestData> const &suggestDataList);

  void removeSuggestion(QString const &suggestKey);
  void removeSuggestion(int index);
  void clearSuggestion();
  Q_SIGNAL void suggestionClicked(NXSuggestBox::SuggestData const &suggestData);
};

#endif // NXSUGGESTBOX_H
