#include "T_TableViewModel.h"

#include <QIcon>

T_TableViewModel::T_TableViewModel(QObject *parent)
    : QAbstractTableModel { parent }
{
  _header << QStringLiteral("预览") << QStringLiteral("歌名") << QStringLiteral("歌手") << QStringLiteral("专辑")
          << QStringLiteral("时长");
  QStringList data0;
  QStringList data1;
  QStringList data2;
  QStringList data3;
  QStringList data4;
  QStringList data5;
  QStringList data6;
  QStringList data7;
  QStringList data8;
  data0 << QStringLiteral("夜航星(Night Voyager)") << QStringLiteral("不才/三体宇宙")
        << QStringLiteral("我的三体之章北海传") << QStringLiteral("05:03");
  data1 << QStringLiteral("玫瑰少年") << QStringLiteral("五月天") << QStringLiteral("玫瑰少年")
        << QStringLiteral("03:55");
  data2 << QStringLiteral("Collapsing World(Original Mix)") << QStringLiteral("Lightscape")
        << QStringLiteral("Collapsing World") << QStringLiteral("03:10");
  data3 << QStringLiteral("RAIN MAN (雨人)") << QStringLiteral("AKIHIDE (佐藤彰秀)") << QStringLiteral("RAIN STORY")
        << QStringLiteral("05:37");
  data4 << QStringLiteral("黑暗森林") << QStringLiteral("雲翼星辰") << QStringLiteral("黑暗森林")
        << QStringLiteral("05:47");
  data5 << QStringLiteral("轻(我的三体第四季主题曲)") << QStringLiteral("刘雪茗") << QStringLiteral("我的三体第四季")
        << QStringLiteral("01:59");
  data6 << QStringLiteral("STYX HELIX") << QStringLiteral("MYTH & ROID") << QStringLiteral("STYX HELIX")
        << QStringLiteral("04:51");
  data7 << QStringLiteral("LAST STARDUST") << QStringLiteral("Aimer") << QStringLiteral("DAWN")
        << QStringLiteral("05:18");
  data8 << QStringLiteral("Running In The Dark") << QStringLiteral("MONKEY MAJIK/塞壬唱片")
        << QStringLiteral("Running In The Dark") << QStringLiteral("03:40");
  _dataList.append(data0);
  _dataList.append(data1);
  _dataList.append(data2);
  _dataList.append(data3);
  _dataList.append(data4);
  _dataList.append(data5);
  _dataList.append(data6);
  _dataList.append(data7);
  _dataList.append(data8);

  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/NaightNavigationStar.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/MaVieEnRose.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/CollapsingWorld.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/RainMan.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/DarkForest.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/Light.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/STYXHELIX.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/LASTSTARDUST.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
  _iconList.append(QIcon(QPixmap(QStringLiteral(":/Resource/Image/Model/RunningInTheDark.jpg"))
                             .scaled(38, 38, Qt::KeepAspectRatio, Qt::SmoothTransformation)));
}

T_TableViewModel::~T_TableViewModel()
{
}

int T_TableViewModel::rowCount(QModelIndex const &parent) const
{
  return 100;
}

int T_TableViewModel::columnCount(QModelIndex const &parent) const
{
  return _header.count();
}

QVariant
T_TableViewModel::data(QModelIndex const &index, int role) const
{
  if (role == Qt::DisplayRole && index.column() != 0) {
    return _dataList[index.row() % 9][index.column() - 1];
  } else if (role == Qt::DecorationRole && index.column() == 0) {
    return _iconList[index.row() % 9];
  } else if (role == Qt::DecorationPropertyRole) {
    return Qt::AlignCenter;
  } else if (role == Qt::TextAlignmentRole && index.column() == 4) {
    return Qt::AlignCenter;
  }
  return QVariant();
}

QVariant
T_TableViewModel::headerData(int section, Qt::Orientation orientation, int role) const
{
  if (orientation == Qt::Horizontal && role == Qt::DisplayRole) {
    return _header[section];
  }
  return QAbstractTableModel::headerData(section, orientation, role);
}
