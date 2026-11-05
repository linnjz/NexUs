#include "NXGraphicsItemPrivate.h"

NXGraphicsItemPrivate::NXGraphicsItemPrivate(QObject *parent)
    : QObject(parent)
{
}

NXGraphicsItemPrivate::~NXGraphicsItemPrivate()
{
}

QDataStream & operator<< (QDataStream &stream, NXGraphicsItemPrivate const *data)
{
  stream << data->m_ItemUID;
  stream << data->m_ItemName;
  stream << data->m_MaxLinkPortCount;
  stream << data->m_CurrentLinkPortState;
  stream << data->m_DataRoutes;
  stream << data->m_ItemImage;
  stream << data->m_ItemSelectedImage;
  return stream;
}

QDataStream & operator>> (QDataStream &stream, NXGraphicsItemPrivate *data)
{
  stream >> data->m_ItemUID;
  stream >> data->m_ItemName;
  stream >> data->m_MaxLinkPortCount;
  stream >> data->m_CurrentLinkPortState;
  stream >> data->m_DataRoutes;
  stream >> data->m_ItemImage;
  stream >> data->m_ItemSelectedImage;
  return stream;
}
