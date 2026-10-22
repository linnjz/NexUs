#if defined(_MSC_VER)
#  pragma warning(push)
#  pragma warning(disable : 5103)
#endif

#ifndef NXPROPERTY_H
#  define NXPROPERTY_H

#  include <cstdlib>
#  include <iostream>
#  include <source_location>
#  include <type_traits>
#  include <utility>

#  include <QObject>
#  include <QtCore/QGlobal.h>
#  include <QtCore/QMetaObject.h>

#  include "LinnUtils.h"

#  ifdef NEXUS_LIBRARY
#    define NX_EXPORT Q_DECL_EXPORT
#  else
#    define NX_EXPORT Q_DECL_IMPORT
#  endif

// PROPERTY: 需要和信号关联
// PRIVATE: 不需要和信号关联
// _H/_CPP: Pimpl版本
//
// 基础版本 - 支持自定义setter和getter类型

#  pragma region PROPERTY_STRATEGY

#  define QS_TAG_DEFAULT_VALUE 10

#  define QS_TAG_ASSIGN_MOVE 20

#  define QS_TAG_CONTRACT_ALWAYS 30
#  define QS_TAG_CONTRACT_DEBUG  31

#  define QS_TAG_NOEXCEPT 40

#  define QS_TAG_SIGNAL_PARAMS  50
#  define QS_TAG_NO_EMIT_SIGNAL 51

#  define QS_TAG_UPDATE 60

#  define QS_TAG_SET_VAL  100
#  define QS_TAG_SET_CREF 101

#  define QS_DEFAULT_VALUE(val)   LINN_PAIR(QS_TAG_DEFAULT_VALUE, val)
#  define QS_ASSIGN_MOVE(name)    LINN_PAIR(QS_TAG_ASSIGN_MOVE, name)
#  define QS_CONTRACT_ALWAYS(...) LINN_PAIR(QS_TAG_CONTRACT_ALWAYS, (__VA_ARGS__))
#  define QS_CONTRACT_DEBUG(...)  LINN_PAIR(QS_TAG_CONTRACT_DEBUG, (__VA_ARGS__))
#  define QS_NOEXCEPT(boolean)    LINN_PAIR(QS_TAG_NOEXCEPT, boolean)
#  define QS_SIGNAL_PARAMS(...)   LINN_PAIR(QS_TAG_SIGNAL_PARAMS, (__VA_ARGS__))
#  define QS_NO_EMIT_SIGNAL       QS_TAG_NO_EMIT_SIGNAL
#  define QS_UPDATE               QS_TAG_UPDATE
#  define QS_SET_VAL(T)           LINN_PAIR(QS_TAG_SET_VAL, T)
#  define QS_SET_CREF(T)          LINN_PAIR(QS_TAG_SET_CREF, T)

#  pragma endregion PROPERTY_STRATEGY


#  pragma region PROPERTY_DEFINITIONS

/*
 * Generic strategy dispatch — shared by all six entry points.
 * Two patterns: (TYPE, NAME, ...) and (CLASS, TYPE, NAME, ...) for CPP variants.
 */
#  define Q_CREATE_DISPATCH(PREFIX, TYPE, NAME, ...) \
    LINN_CAT(Q_CREATE_DISPATCH_, LINN_IS_PAREN(LINN_EXPAND(TYPE)))(PREFIX, TYPE, NAME, ##__VA_ARGS__)

#  define Q_CREATE_DISPATCH_0(PREFIX, TYPE, NAME, ...) PREFIX(TYPE, TYPE, NAME, ##__VA_ARGS__)

#  define Q_CREATE_DISPATCH_1(PREFIX, TYPE, NAME, ...) \
    Q_CREATE_STRATEGY(PREFIX, LINN_PAIR_FIRST(TYPE), LINN_PAIR_SECOND(TYPE), NAME, ##__VA_ARGS__)

#  define Q_CREATE_STRATEGY(PREFIX, tag, T, NAME, ...) LINN_CAT(Q_CREATE_STRATEGY_, tag)(PREFIX, T, NAME, ##__VA_ARGS__)

#  define Q_CREATE_STRATEGY_100(PREFIX, T, NAME, ...) PREFIX(T, T, NAME, ##__VA_ARGS__)

#  define Q_CREATE_STRATEGY_101(PREFIX, T, NAME, ...) PREFIX(const T &, T, NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_DISPATCH(PREFIX, CLASS, TYPE, NAME, ...) \
    LINN_CAT(Q_CREATE_CPP_DISPATCH_, LINN_IS_PAREN(LINN_EXPAND(TYPE)))(PREFIX, CLASS, TYPE, NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_DISPATCH_0(PREFIX, CLASS, TYPE, NAME, ...) PREFIX(CLASS, TYPE, TYPE, NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_DISPATCH_1(PREFIX, CLASS, TYPE, NAME, ...) \
    Q_CREATE_CPP_STRATEGY(PREFIX, CLASS, LINN_PAIR_FIRST(TYPE), LINN_PAIR_SECOND(TYPE), NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_STRATEGY(PREFIX, CLASS, tag, T, NAME, ...) \
    LINN_CAT(Q_CREATE_CPP_STRATEGY_, tag)(PREFIX, CLASS, T, NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_STRATEGY_100(PREFIX, CLASS, T, NAME, ...) PREFIX(CLASS, T, T, NAME, ##__VA_ARGS__)

#  define Q_CREATE_CPP_STRATEGY_101(PREFIX, CLASS, T, NAME, ...) PREFIX(CLASS, const T &, T, NAME, ##__VA_ARGS__)

/*
 * DEFINITION
 */
#  define Q_PROPERTY_CREATE(TYPE, NAME, ...) Q_CREATE_DISPATCH(Q_PROPERTY_CREATE_2, TYPE, NAME, ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE(TYPE, NAME, ...) Q_CREATE_DISPATCH(Q_PRIVATE_CREATE_2, TYPE, NAME, ##__VA_ARGS__)

/*
 * DEFINITION-SEPARATION
 */
#  define Q_PROPERTY_CREATE_H(TYPE, NAME, ...) Q_CREATE_DISPATCH(Q_PROPERTY_CREATE_2_H, TYPE, NAME, ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE_H(TYPE, NAME, ...) Q_CREATE_DISPATCH(Q_PRIVATE_CREATE_2_H, TYPE, NAME, ##__VA_ARGS__)

#  define Q_PROPERTY_CREATE_CPP(CLASS, TYPE, NAME, ...) \
    Q_CREATE_CPP_DISPATCH(Q_PROPERTY_CREATE_2_CPP, CLASS, TYPE, NAME, ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE_CPP(CLASS, TYPE, NAME, ...) \
    Q_CREATE_CPP_DISPATCH(Q_PRIVATE_CREATE_2_CPP, CLASS, TYPE, NAME, ##__VA_ARGS__)

/*
 * CUSTOM-SETTER-GETTER-TYPE
 */
#  define Q_PROPERTY_CREATE_2(SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PROPERTY_CREATE_2_FROM_PAIR(SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE_2(SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PRIVATE_CREATE_2_FROM_PAIR(SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)

#  define Q_PROPERTY_CREATE_2_H(SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PROPERTY_CREATE_2_H_FROM_PAIR(SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)

#  define Q_PROPERTY_CREATE_2_CPP(CLASS, SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PROPERTY_CREATE_2_CPP_FROM_PAIR(CLASS, SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE_2_H(SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PRIVATE_CREATE_2_H_FROM_PAIR(SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)

#  define Q_PRIVATE_CREATE_2_CPP(CLASS, SET_TYPE, GET_TYPE, NAME, ...) \
    Q_PRIVATE_CREATE_2_CPP_FROM_PAIR(CLASS, SET_TYPE, GET_TYPE, Q_PROPERTY_NAME_PAIR(NAME), ##__VA_ARGS__)


/*
 * PROPERTY-INNER-IMPL
 */
#  define Q_PROPERTY_CREATE_2_FROM_PAIR(SET_TYPE, GET_TYPE, NAME_PAIR, ...)                                                     \
    Q_PROPERTY_CREATE_2_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE,                      /*setName(SET_TYPE Name) GET_TYPE getName()*/ \
                                         Q_PROP_PARAMETER_NAME(NAME_PAIR),        /*Name*/                                      \
                                         Q_PROP_PARAMETER_ASSIGN_MODE(NAME_PAIR), /*MODE: = std::move(Name) / = Name*/          \
                                         Q_PROP_PROPERTY_NAME(NAME_PAIR),         /*pName*/                                     \
                                         Q_PROP_MEMBER_NAME(NAME_PAIR),           /*_pName*/                                    \
                                         Q_PROP_SETTER_NAME(NAME_PAIR),           /*setName*/                                   \
                                         Q_PROP_GETTER_NAME(NAME_PAIR),           /*getName*/                                   \
                                         Q_PROP_SIGNAL_NAME(NAME_PAIR),           /*pNameChanged*/                              \
                                         __VA_ARGS__)

#  define Q_PROPERTY_CREATE_2_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE, PARAMETER_NAME, ASSIGN_MODE, PROPERTY_NAME, MEMBER_NAME, SETTER_NAME, GETTER_NAME, SIGNAL_NAME, ...) \
                                                                                                                                                                        \
    Q_PROPERTY(std::remove_cvref_t<SET_TYPE> PROPERTY_NAME MEMBER MEMBER_NAME NOTIFY SIGNAL_NAME)                                                                       \
                                                                                                                                                                        \
  public:                                                                                                                                                               \
    Q_SIGNAL void SIGNAL_NAME(Q_SIGNAL_PARAMS_CONTENT_FROM_ARGS(__VA_ARGS__));                                                                                          \
                                                                                                                                                                        \
    void SETTER_NAME(SET_TYPE PARAMETER_NAME) noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                                         \
    {                                                                                                                                                                   \
      Q_CONTRACT_CHECK_FROM_ARGS(PARAMETER_NAME, ##__VA_ARGS__);                                                                                                        \
      MEMBER_NAME = Q_ASSIGN_VALUE(PARAMETER_NAME, Q_ASSIGN_MOVE_ENABLED(ASSIGN_MODE, __VA_ARGS__));                                                                    \
      Q_SETTER_UPDATE_FROM_ARGS(__VA_ARGS__);                                                                                                                           \
      Q_SETTER_EMIT_FROM_ARGS(PARAMETER_NAME, MEMBER_NAME, ##__VA_ARGS__);                                                                                              \
    }                                                                                                                                                                   \
                                                                                                                                                                        \
    [[nodiscard]] GET_TYPE GETTER_NAME() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                                        \
    {                                                                                                                                                                   \
      return MEMBER_NAME;                                                                                                                                               \
    }                                                                                                                                                                   \
                                                                                                                                                                        \
  private:                                                                                                                                                              \
    std::remove_cvref_t<SET_TYPE> MEMBER_NAME Q_MEMBER_INIT_FROM_ARGS(__VA_ARGS__);


/*
 * PRIVATE-INNER-IMPL
 */
#  define Q_PRIVATE_CREATE_2_FROM_PAIR(SET_TYPE, GET_TYPE, NAME_PAIR, ...)                                                     \
    Q_PRIVATE_CREATE_2_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE,                      /*setName(SET_TYPE Name) GET_TYPE getName()*/ \
                                        Q_PROP_PARAMETER_NAME(NAME_PAIR),        /*Name*/                                      \
                                        Q_PROP_PARAMETER_ASSIGN_MODE(NAME_PAIR), /*MODE: = std::move(Name) / = Name*/          \
                                        Q_PROP_MEMBER_NAME(NAME_PAIR),           /*_pName*/                                    \
                                        Q_PROP_SETTER_NAME(NAME_PAIR),           /*setName*/                                   \
                                        Q_PROP_GETTER_NAME(NAME_PAIR),           /*getName*/                                   \
                                        __VA_ARGS__)

#  define Q_PRIVATE_CREATE_2_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE, PARAMETER_NAME, ASSIGN_MODE, MEMBER_NAME, SETTER_NAME, GETTER_NAME, ...) \
                                                                                                                                           \
  public:                                                                                                                                  \
    void SETTER_NAME(SET_TYPE PARAMETER_NAME) noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                            \
    {                                                                                                                                      \
      Q_CONTRACT_CHECK_FROM_ARGS(PARAMETER_NAME, ##__VA_ARGS__);                                                                           \
      MEMBER_NAME = Q_ASSIGN_VALUE(PARAMETER_NAME, Q_ASSIGN_MOVE_ENABLED(ASSIGN_MODE, __VA_ARGS__));                                       \
      Q_SETTER_UPDATE_FROM_ARGS(__VA_ARGS__);                                                                                              \
    }                                                                                                                                      \
                                                                                                                                           \
    [[nodiscard]] GET_TYPE GETTER_NAME() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                           \
    {                                                                                                                                      \
      return MEMBER_NAME;                                                                                                                  \
    }                                                                                                                                      \
                                                                                                                                           \
  private:                                                                                                                                 \
    std::remove_cvref_t<SET_TYPE> MEMBER_NAME Q_MEMBER_INIT_FROM_ARGS(__VA_ARGS__);


/*
 * PROPERTY-INNER-IMPL-H
 */
#  define Q_PROPERTY_CREATE_2_H_FROM_PAIR(SET_TYPE, GET_TYPE, NAME_PAIR, ...)                                              \
    Q_PROPERTY_CREATE_2_H_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE,               /*setName(SET_TYPE Name) GET_TYPE getName()*/ \
                                           Q_PROP_PARAMETER_NAME(NAME_PAIR), /*Name*/                                      \
                                           Q_PROP_PROPERTY_NAME(NAME_PAIR),  /*pName*/                                     \
                                           Q_PROP_SETTER_NAME(NAME_PAIR),    /*setName*/                                   \
                                           Q_PROP_GETTER_NAME(NAME_PAIR),    /*getName*/                                   \
                                           Q_PROP_SIGNAL_NAME(NAME_PAIR),    /*pNameChanged*/                              \
                                           __VA_ARGS__)

#  define Q_PROPERTY_CREATE_2_H_FROM_PAIR_UNPACK(SET_TYPE, GET_TYPE, PARAMETER_NAME, PROPERTY_NAME, SETTER_NAME, GETTER_NAME, SIGNAL_NAME, ...) \
                                                                                                                                                \
    Q_PROPERTY(std::remove_cvref_t<SET_TYPE> PROPERTY_NAME READ GETTER_NAME WRITE SETTER_NAME NOTIFY SIGNAL_NAME)                               \
                                                                                                                                                \
  public:                                                                                                                                       \
    Q_SIGNAL void SIGNAL_NAME(Q_SIGNAL_PARAMS_CONTENT_FROM_ARGS(__VA_ARGS__));                                                                  \
    void SETTER_NAME(SET_TYPE PARAMETER_NAME) noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__));                                                \
    [[nodiscard]] GET_TYPE GETTER_NAME() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__));


/*
 * PROPERTY-INNER-IMPL-CPP
 */
#  define Q_PROPERTY_CREATE_2_CPP_FROM_PAIR(CLASS, SET_TYPE, GET_TYPE, NAME_PAIR, ...)         \
    Q_PROPERTY_CREATE_2_CPP_FROM_PAIR_UNPACK(                                                  \
        CLASS, SET_TYPE, GET_TYPE,               /*setName(SET_TYPE Name) GET_TYPE getName()*/ \
        Q_PROP_PARAMETER_NAME(NAME_PAIR),        /*Name*/                                      \
        Q_PROP_PARAMETER_ASSIGN_MODE(NAME_PAIR), /*MODE: = std::move(Name) / = Name*/          \
        Q_PROP_MEMBER_NAME(NAME_PAIR),           /*_pName*/                                    \
        Q_PROP_SETTER_NAME(NAME_PAIR),           /*setName*/                                   \
        Q_PROP_GETTER_NAME(NAME_PAIR),           /*getName*/                                   \
        __VA_ARGS__)

#  define Q_PROPERTY_CREATE_2_CPP_FROM_PAIR_UNPACK(CLASS, SET_TYPE, GET_TYPE, PARAMETER_NAME, ASSIGN_MODE, MEMBER_NAME, SETTER_NAME, GETTER_NAME, ...) \
                                                                                                                                                       \
    void CLASS::SETTER_NAME(SET_TYPE PARAMETER_NAME) noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                 \
    {                                                                                                                                                  \
      Q_D(CLASS);                                                                                                                                      \
      Q_CONTRACT_CHECK_FROM_ARGS(PARAMETER_NAME, ##__VA_ARGS__);                                                                                       \
      d->MEMBER_NAME = Q_ASSIGN_VALUE(PARAMETER_NAME, Q_ASSIGN_MOVE_ENABLED(ASSIGN_MODE, __VA_ARGS__));                                                \
      Q_SETTER_UPDATE_FROM_ARGS(__VA_ARGS__);                                                                                                          \
      Q_SETTER_EMIT_FROM_ARGS(PARAMETER_NAME, d->MEMBER_NAME, ##__VA_ARGS__);                                                                          \
    }                                                                                                                                                  \
                                                                                                                                                       \
    GET_TYPE CLASS::GETTER_NAME() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                              \
    {                                                                                                                                                  \
      return d_ptr->MEMBER_NAME;                                                                                                                       \
    }


/*
 * PRIVATE-INNER-IMPL-H
 */
#  define Q_PRIVATE_CREATE_2_H_FROM_PAIR(SET_TYPE, GET_TYPE, NAME_PAIR, ...)                \
                                                                                            \
  public:                                                                                   \
    void Q_PROP_SETTER_NAME(NAME_PAIR)(SET_TYPE Q_PROP_PARAMETER_NAME(NAME_PAIR)) noexcept( \
        Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__));                                           \
    [[nodiscard]] GET_TYPE Q_PROP_GETTER_NAME(NAME_PAIR)() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__));


/*
 * PRIVATE-INNER-IMPL-CPP
 */
#  define Q_PRIVATE_CREATE_2_CPP_FROM_PAIR(CLASS, SET_TYPE, GET_TYPE, NAME_PAIR, ...)          \
    Q_PRIVATE_CREATE_2_CPP_FROM_PAIR_UNPACK(                                                   \
        CLASS, SET_TYPE, GET_TYPE,               /*setName(SET_TYPE Name) GET_TYPE getName()*/ \
        Q_PROP_PARAMETER_NAME(NAME_PAIR),        /*Name*/                                      \
        Q_PROP_PARAMETER_ASSIGN_MODE(NAME_PAIR), /*MODE: = std::move(Name) / = Name*/          \
        Q_PROP_MEMBER_NAME(NAME_PAIR),           /*_pName*/                                    \
        Q_PROP_SETTER_NAME(NAME_PAIR),           /*setName*/                                   \
        Q_PROP_GETTER_NAME(NAME_PAIR),           /*getName*/                                   \
        __VA_ARGS__)

#  define Q_PRIVATE_CREATE_2_CPP_FROM_PAIR_UNPACK(CLASS, SET_TYPE, GET_TYPE, PARAMETER_NAME, ASSIGN_MODE, MEMBER_NAME, SETTER_NAME, GETTER_NAME, ...) \
                                                                                                                                                      \
    void CLASS::SETTER_NAME(SET_TYPE PARAMETER_NAME) noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                \
    {                                                                                                                                                 \
      Q_D(CLASS);                                                                                                                                     \
      Q_CONTRACT_CHECK_FROM_ARGS(PARAMETER_NAME, ##__VA_ARGS__);                                                                                      \
      d->MEMBER_NAME = Q_ASSIGN_VALUE(PARAMETER_NAME, Q_ASSIGN_MOVE_ENABLED(ASSIGN_MODE, __VA_ARGS__));                                               \
      Q_SETTER_UPDATE_FROM_ARGS(__VA_ARGS__);                                                                                                         \
    }                                                                                                                                                 \
                                                                                                                                                      \
    GET_TYPE CLASS::GETTER_NAME() const noexcept(Q_NOEXCEPT_VALUE_FROM_ARGS(__VA_ARGS__))                                                             \
    {                                                                                                                                                 \
      return d_ptr->MEMBER_NAME;                                                                                                                      \
    }


/*
 * MEMBER
 */
#  define Q_PROPERTY_CREATE_D(TYPE, NAME, ...) \
                                               \
  private:                                     \
    TYPE _p##NAME __VA_OPT__(=) __VA_ARGS__;

#  define Q_PRIVATE_CREATE_D(TYPE, NAME, ...) \
                                              \
  private:                                    \
    TYPE _p##NAME __VA_OPT__(=) __VA_ARGS__;


/*
 * INNER-PIMPL
 */
#  define Q_Q_CREATE(CLASS)                                      \
                                                                 \
  protected:                                                     \
    explicit CLASS(CLASS##Private &dd, CLASS *parent = nullptr); \
    QScopedPointer<CLASS##Private> d_ptr;                        \
                                                                 \
  private:                                                       \
    Q_DISABLE_COPY(CLASS)                                        \
    Q_DECLARE_PRIVATE(CLASS)

#  define Q_D_CREATE(CLASS) \
                            \
  protected:                \
    CLASS *q_ptr;           \
                            \
  private:                  \
    Q_DECLARE_PUBLIC(CLASS);

#  pragma endregion PROPERTY_DEFINITIONS


#  pragma region ENUM_DEFINITIONS

#  define Q_BEGIN_ENUM_CREATE_IMPL(_1, _2, NAME, ...) NAME
#  define Q_BEGIN_ENUM_CREATE_ARGS(args)              Q_BEGIN_ENUM_CREATE_IMPL args
#  define Q_BEGIN_ENUM_CREATE(...) \
    Q_BEGIN_ENUM_CREATE_ARGS((__VA_ARGS__, Q_BEGIN_ENUM_CREATE_2, Q_BEGIN_ENUM_CREATE_1))(__VA_ARGS__)

// 枚举类导出  兼容QT5低版本
#  if QT_VERSION >= QT_VERSION_CHECK(5, 14, 0)
#    define Q_BEGIN_ENUM_CREATE_1(CLASS) \
      namespace CLASS                    \
      {                                  \
      Q_NAMESPACE_EXPORT()

#    define Q_BEGIN_ENUM_CREATE_2(CLASS, EXPORT) \
      namespace CLASS                            \
      {                                          \
      Q_NAMESPACE_EXPORT(EXPORT)

#    define Q_END_ENUM_CREATE(CLASS) }

#    define Q_ENUM_CREATE(CLASS) Q_ENUM_NS(CLASS)
#  else
#    define Q_BEGIN_ENUM_CREATE_1(CLASS) \
      class CLASS : public QObject       \
      {                                  \
        Q_OBJECT                         \
                                         \
      public:

#    define Q_BEGIN_ENUM_CREATE_2(CLASS, EXPORT) \
      class EXPORT CLASS : public QObject        \
      {                                          \
        Q_OBJECT                                 \
                                                 \
      public:

#    define Q_END_ENUM_CREATE(CLASS) \
                                     \
    private:                         \
      Q_DISABLE_COPY(CLASS)          \
      }                              \
      ;

#    define Q_ENUM_CREATE(CLASS) Q_ENUM(CLASS)
#  endif

template <typename EnumT>
QString
qEnumNameFromType() noexcept
{
  using Enum = std::decay_t<EnumT>;
  static_assert(std::is_enum_v<Enum>, "qEnumName requires an enum type");

  QMetaEnum meta = QMetaEnum::fromType<Enum>();
  if (!meta.isValid()) {
    return {};
  }
  return QString::fromLatin1(meta.name());
}

template <typename EnumT>
QString
qEnumNameFromValue(EnumT value) noexcept
{
  using Enum = std::decay_t<EnumT>;
  static_assert(std::is_enum_v<Enum>, "qEnumName requires an enum type");

  const QMetaEnum metaEnum = QMetaEnum::fromType<Enum>();
  if (!metaEnum.isValid()) {
    return {};
  }

  using Underlying   = std::underlying_type_t<Enum>;
  const int intValue = static_cast<int>(static_cast<Underlying>(value));
  const char *key    = metaEnum.valueToKey(intValue);
  if (!key) {
    return QString::number(intValue);
  }
  return QString::fromLatin1(key);
}

#  pragma endregion ENUM_DEFINITIONS


#  pragma region PROPERTY_HELPER_CORE

#  define Q_EMPTY()
#  define Q_EMPTY_TOKEN    Q_EMPTY()
#  define Q_UNPAREN_I(...) __VA_ARGS__
#  define Q_UNPAREN(x)     Q_UNPAREN_I x

#  define Q_SCAN_BOOL(...)                                LINN_CAT(Q_SCAN_BOOL_, LINN_NARGS(__VA_ARGS__))(__VA_ARGS__)
#  define Q_SCAN_BOOL_1(PRED)                             0
#  define Q_SCAN_BOOL_2(PRED, a1)                         PRED(a1)
#  define Q_SCAN_BOOL_3(PRED, a1, a2)                     LINN_OR(PRED(a1), Q_SCAN_BOOL_2(PRED, a2))
#  define Q_SCAN_BOOL_4(PRED, a1, a2, a3)                 LINN_OR(PRED(a1), Q_SCAN_BOOL_3(PRED, a2, a3))
#  define Q_SCAN_BOOL_5(PRED, a1, a2, a3, a4)             LINN_OR(PRED(a1), Q_SCAN_BOOL_4(PRED, a2, a3, a4))
#  define Q_SCAN_BOOL_6(PRED, a1, a2, a3, a4, a5)         LINN_OR(PRED(a1), Q_SCAN_BOOL_5(PRED, a2, a3, a4, a5))
#  define Q_SCAN_BOOL_7(PRED, a1, a2, a3, a4, a5, a6)     LINN_OR(PRED(a1), Q_SCAN_BOOL_6(PRED, a2, a3, a4, a5, a6))
#  define Q_SCAN_BOOL_8(PRED, a1, a2, a3, a4, a5, a6, a7) LINN_OR(PRED(a1), Q_SCAN_BOOL_7(PRED, a2, a3, a4, a5, a6, a7))
#  define Q_SCAN_BOOL_9(PRED, a1, a2, a3, a4, a5, a6, a7, a8) \
    LINN_OR(PRED(a1), Q_SCAN_BOOL_8(PRED, a2, a3, a4, a5, a6, a7, a8))

#  define Q_SCAN_FIRST(...)                        LINN_CAT(Q_SCAN_FIRST_, LINN_NARGS(__VA_ARGS__))(__VA_ARGS__)
#  define Q_SCAN_FIRST_3(PRED, VALUE, DEFAULT)     DEFAULT
#  define Q_SCAN_FIRST_4(PRED, VALUE, DEFAULT, a1) LINN_IF(PRED(a1))(VALUE(a1), DEFAULT)
#  define Q_SCAN_FIRST_5(PRED, VALUE, DEFAULT, a1, a2) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_4(PRED, VALUE, DEFAULT, a2))
#  define Q_SCAN_FIRST_6(PRED, VALUE, DEFAULT, a1, a2, a3) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_5(PRED, VALUE, DEFAULT, a2, a3))
#  define Q_SCAN_FIRST_7(PRED, VALUE, DEFAULT, a1, a2, a3, a4) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_6(PRED, VALUE, DEFAULT, a2, a3, a4))
#  define Q_SCAN_FIRST_8(PRED, VALUE, DEFAULT, a1, a2, a3, a4, a5) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_7(PRED, VALUE, DEFAULT, a2, a3, a4, a5))
#  define Q_SCAN_FIRST_9(PRED, VALUE, DEFAULT, a1, a2, a3, a4, a5, a6) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_8(PRED, VALUE, DEFAULT, a2, a3, a4, a5, a6))
#  define Q_SCAN_FIRST_10(PRED, VALUE, DEFAULT, a1, a2, a3, a4, a5, a6, a7) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_9(PRED, VALUE, DEFAULT, a2, a3, a4, a5, a6, a7))
#  define Q_SCAN_FIRST_11(PRED, VALUE, DEFAULT, a1, a2, a3, a4, a5, a6, a7, a8) \
    LINN_IF(PRED(a1))(VALUE(a1), Q_SCAN_FIRST_10(PRED, VALUE, DEFAULT, a2, a3, a4, a5, a6, a7, a8))

#  define Q_CONTRACT_ASSERT_DEBUG(...)  LINN_CAT(Q_CONTRACT_ASSERT_DEBUG_, LINN_NARGS(__VA_ARGS__))(__VA_ARGS__)
#  define Q_CONTRACT_ASSERT_ALWAYS(...) LINN_CAT(Q_CONTRACT_ASSERT_ALWAYS_, LINN_NARGS(__VA_ARGS__))(__VA_ARGS__)

#  if defined(DEBUG) || defined(_DEBUG)
#    define Q_CONTRACT_ASSERT_DEBUG_2(expr, msg) Q_CONTRACT_ASSERT_DEBUG_3(expr, msg, std::source_location::current())
#    define Q_CONTRACT_ASSERT_DEBUG_3(expr, msg, location)                                        \
      do {                                                                                        \
        if (!(expr)) [[unlikely]] {                                                               \
          qCritical() << "[CONTRACT VIOLATION] " << (msg) << "\n"                                 \
                      << "  Condition: " << #expr << "\n"                                         \
                      << "  File: " << (location).file_name() << ":" << (location).line() << "\n" \
                      << "  Function: " << (location).function_name() << "\n";                    \
          std::abort();                                                                           \
        }                                                                                         \
      } while (0)
#  else
#    define Q_CONTRACT_ASSERT_DEBUG_2(expr, msg)           ((void) 0)
#    define Q_CONTRACT_ASSERT_DEBUG_3(expr, msg, location) ((void) 0)
#  endif

#  define Q_CONTRACT_ASSERT_ALWAYS_2(expr, msg) Q_CONTRACT_ASSERT_ALWAYS_3(expr, msg, std::source_location::current())
#  if defined(NDEBUG) || defined(_NDEBUG)
#    define Q_CONTRACT_ASSERT_ALWAYS_3(expr, msg, location)       \
      do {                                                        \
        if (!(expr)) [[unlikely]] {                               \
          qCritical() << "[CONTRACT VIOLATION] " << (msg) << "\n" \
                      << "  Condition: " << #expr << "\n";        \
          std::abort();                                           \
        }                                                         \
      } while (0)
#  else
#    define Q_CONTRACT_ASSERT_ALWAYS_3(expr, msg, location)                                       \
      do {                                                                                        \
        if (!(expr)) [[unlikely]] {                                                               \
          qCritical() << "[CONTRACT VIOLATION] " << (msg) << "\n"                                 \
                      << "  Condition: " << #expr << "\n"                                         \
                      << "  File: " << (location).file_name() << ":" << (location).line() << "\n" \
                      << "  Function: " << (location).function_name() << "\n";                    \
          std::abort();                                                                           \
        }                                                                                         \
      } while (0)
#  endif

#  define Q_PROPERTY_NAME_COPY_TAG 0

#  define Q_PROPERTY_NAME_PAIR(NAME) \
    LINN_IF(LINN_IS_PAREN(LINN_EXPAND(NAME)))(LINN_EXPAND(NAME), LINN_PAIR(Q_PROPERTY_NAME_COPY_TAG, NAME))
#  define Q_PROP_PARAMETER_NAME(NAME_PAIR)        LINN_PAIR_SECOND(NAME_PAIR)
#  define Q_PROP_PARAMETER_ASSIGN_MODE(NAME_PAIR) Q_IS_TAG_ASSIGN_MOVE(LINN_PAIR_FIRST(NAME_PAIR))
#  define Q_PROP_PROPERTY_NAME(NAME_PAIR)         LINN_CAT(p, Q_PROP_PARAMETER_NAME(NAME_PAIR))
#  define Q_PROP_MEMBER_NAME(NAME_PAIR)           LINN_CAT(_p, Q_PROP_PARAMETER_NAME(NAME_PAIR))
#  define Q_PROP_SETTER_NAME(NAME_PAIR)           LINN_CAT(set, Q_PROP_PARAMETER_NAME(NAME_PAIR))
#  define Q_PROP_GETTER_NAME(NAME_PAIR)           LINN_CAT(get, Q_PROP_PARAMETER_NAME(NAME_PAIR))
#  define Q_PROP_SIGNAL_NAME(NAME_PAIR)           LINN_CAT3(p, Q_PROP_PARAMETER_NAME(NAME_PAIR), Changed)

#  define Q_TAG_IS_DEFAULT_10         LINN_PROBE(~)
#  define Q_TAG_IS_ASSIGN_MOVE_20     LINN_PROBE(~)
#  define Q_TAG_IS_CONTRACT_ALWAYS_30 LINN_PROBE(~)
#  define Q_TAG_IS_CONTRACT_DEBUG_31  LINN_PROBE(~)
#  define Q_TAG_IS_NOEXCEPT_40        LINN_PROBE(~)
#  define Q_TAG_IS_SIGNAL_PARAMS_50   LINN_PROBE(~)
#  define Q_TAG_IS_NO_EMIT_SIGNAL_51  LINN_PROBE(~)
#  define Q_TAG_IS_UPDATE_60          LINN_PROBE(~)
#  define Q_TAG_IS_SET_VAL_100        LINN_PROBE(~)
#  define Q_TAG_IS_SET_CREF_101       LINN_PROBE(~)

#  define Q_IS_TAG_DEFAULT(tag)         LINN_CHECK(LINN_CAT(Q_TAG_IS_DEFAULT_, tag))
#  define Q_IS_TAG_ASSIGN_MOVE(tag)     LINN_CHECK(LINN_CAT(Q_TAG_IS_ASSIGN_MOVE_, tag))
#  define Q_IS_TAG_CONTRACT_ALWAYS(tag) LINN_CHECK(LINN_CAT(Q_TAG_IS_CONTRACT_ALWAYS_, tag))
#  define Q_IS_TAG_CONTRACT_DEBUG(tag)  LINN_CHECK(LINN_CAT(Q_TAG_IS_CONTRACT_DEBUG_, tag))
#  define Q_IS_TAG_NOEXCEPT(tag)        LINN_CHECK(LINN_CAT(Q_TAG_IS_NOEXCEPT_, tag))
#  define Q_IS_TAG_SIGNAL_PARAMS(tag)   LINN_CHECK(LINN_CAT(Q_TAG_IS_SIGNAL_PARAMS_, tag))
#  define Q_IS_TAG_NO_EMIT_SIGNAL(tag)  LINN_CHECK(LINN_CAT(Q_TAG_IS_NO_EMIT_SIGNAL_, tag))
#  define Q_IS_TAG_UPDATE(tag)          LINN_CHECK(LINN_CAT(Q_TAG_IS_UPDATE_, tag))

#  define Q_ASSIGN_MOVE_TOKEN_QS_ASSIGN_MOVE LINN_PROBE(~)
#  define Q_IS_ASSIGN_MOVE_TOKEN(x)          LINN_CHECK(LINN_CAT(Q_ASSIGN_MOVE_TOKEN_, x))

#  define Q_ARG_IS_DEFAULT(arg)   LINN_CAT(Q_ARG_IS_DEFAULT_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_DEFAULT_0(arg) 0
#  define Q_ARG_IS_DEFAULT_1(arg) Q_IS_TAG_DEFAULT(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_ASSIGN_MOVE(arg)   LINN_CAT(Q_ARG_IS_ASSIGN_MOVE_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_ASSIGN_MOVE_0(arg) LINN_OR(Q_IS_TAG_ASSIGN_MOVE(arg), Q_IS_ASSIGN_MOVE_TOKEN(arg))
#  define Q_ARG_IS_ASSIGN_MOVE_1(arg) Q_IS_TAG_ASSIGN_MOVE(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_CONTRACT_ALWAYS(arg)   LINN_CAT(Q_ARG_IS_CONTRACT_ALWAYS_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_CONTRACT_ALWAYS_0(arg) 0
#  define Q_ARG_IS_CONTRACT_ALWAYS_1(arg) Q_IS_TAG_CONTRACT_ALWAYS(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_CONTRACT_DEBUG(arg)   LINN_CAT(Q_ARG_IS_CONTRACT_DEBUG_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_CONTRACT_DEBUG_0(arg) 0
#  define Q_ARG_IS_CONTRACT_DEBUG_1(arg) Q_IS_TAG_CONTRACT_DEBUG(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_NOEXCEPT(arg)   LINN_CAT(Q_ARG_IS_NOEXCEPT_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_NOEXCEPT_0(arg) 0
#  define Q_ARG_IS_NOEXCEPT_1(arg) Q_IS_TAG_NOEXCEPT(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_SIGNAL_PARAMS(arg)   LINN_CAT(Q_ARG_IS_SIGNAL_PARAMS_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_SIGNAL_PARAMS_0(arg) 0
#  define Q_ARG_IS_SIGNAL_PARAMS_1(arg) Q_IS_TAG_SIGNAL_PARAMS(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_NO_EMIT_SIGNAL(arg)   LINN_CAT(Q_ARG_IS_NO_EMIT_SIGNAL_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_NO_EMIT_SIGNAL_0(arg) Q_IS_TAG_NO_EMIT_SIGNAL(arg)
#  define Q_ARG_IS_NO_EMIT_SIGNAL_1(arg) Q_IS_TAG_NO_EMIT_SIGNAL(LINN_PAIR_FIRST(arg))

#  define Q_ARG_IS_UPDATE(arg)   LINN_CAT(Q_ARG_IS_UPDATE_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_IS_UPDATE_0(arg) Q_IS_TAG_UPDATE(arg)
#  define Q_ARG_IS_UPDATE_1(arg) Q_IS_TAG_UPDATE(LINN_PAIR_FIRST(arg))

#  define Q_ARG_DEFAULT_VALUE(arg) LINN_CAT(Q_ARG_DEFAULT_VALUE_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_DEFAULT_VALUE_0(arg)
#  define Q_ARG_DEFAULT_VALUE_1(arg) LINN_IF(Q_IS_TAG_DEFAULT(LINN_PAIR_FIRST(arg)))(LINN_PAIR_SECOND(arg), )

#  define Q_ARG_NOEXCEPT_VALUE(arg)   LINN_CAT(Q_ARG_NOEXCEPT_VALUE_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_NOEXCEPT_VALUE_0(arg) false
#  define Q_ARG_NOEXCEPT_VALUE_1(arg) LINN_IF(Q_IS_TAG_NOEXCEPT(LINN_PAIR_FIRST(arg)))(LINN_PAIR_SECOND(arg), false)

#  define Q_ARG_SIGNAL_PARAMS_CONTENT(arg) LINN_CAT(Q_ARG_SIGNAL_PARAMS_CONTENT_, LINN_IS_PAREN(LINN_EXPAND(arg)))(arg)
#  define Q_ARG_SIGNAL_PARAMS_CONTENT_0(arg)
#  define Q_ARG_SIGNAL_PARAMS_CONTENT_1(arg) \
    LINN_IF(Q_IS_TAG_SIGNAL_PARAMS(LINN_PAIR_FIRST(arg)))(Q_UNPAREN(LINN_PAIR_SECOND(arg)), )

#  define Q_HAS_DEFAULT_FROM_ARGS(...)       Q_SCAN_BOOL(Q_ARG_IS_DEFAULT, ##__VA_ARGS__)
#  define Q_HAS_ASSIGN_MOVE_FROM_ARGS(...)   Q_SCAN_BOOL(Q_ARG_IS_ASSIGN_MOVE, ##__VA_ARGS__)
#  define Q_HAS_SIGNAL_PARAMS_FROM_ARGS(...) Q_SCAN_BOOL(Q_ARG_IS_SIGNAL_PARAMS, ##__VA_ARGS__)
#  define Q_HAS_NO_EMIT_FROM_ARGS(...)       Q_SCAN_BOOL(Q_ARG_IS_NO_EMIT_SIGNAL, ##__VA_ARGS__)
#  define Q_HAS_UPDATE_FROM_ARGS(...)        Q_SCAN_BOOL(Q_ARG_IS_UPDATE, ##__VA_ARGS__)

#  define Q_DEFAULT_VALUE_FROM_ARGS(...) \
    Q_SCAN_FIRST(Q_ARG_IS_DEFAULT, Q_ARG_DEFAULT_VALUE, Q_EMPTY_TOKEN, ##__VA_ARGS__)
#  define Q_NOEXCEPT_VALUE_FROM_ARGS(...) Q_SCAN_FIRST(Q_ARG_IS_NOEXCEPT, Q_ARG_NOEXCEPT_VALUE, false, ##__VA_ARGS__)
#  define Q_SIGNAL_PARAMS_CONTENT_FROM_ARGS(...) \
    Q_SCAN_FIRST(Q_ARG_IS_SIGNAL_PARAMS, Q_ARG_SIGNAL_PARAMS_CONTENT, Q_EMPTY_TOKEN, ##__VA_ARGS__)

#  define Q_ASSIGN_MOVE_ENABLED(name_assign_move, ...) \
    LINN_OR(name_assign_move, Q_HAS_ASSIGN_MOVE_FROM_ARGS(__VA_ARGS__))
#  define Q_ASSIGN_VALUE(value, use_move) LINN_IF(use_move)(std::move(value), value)

#  define Q_MEMBER_INIT_FROM_ARGS(...) \
    LINN_IF(Q_HAS_DEFAULT_FROM_ARGS(__VA_ARGS__))({ Q_DEFAULT_VALUE_FROM_ARGS(__VA_ARGS__) }, )
#  define Q_SETTER_UPDATE_FROM_ARGS(...) LINN_IF(Q_HAS_UPDATE_FROM_ARGS(__VA_ARGS__))(this->update();, )

#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES(value, ...) \
    LINN_CAT(Q_SIGNAL_EMIT_ARGS_FROM_TYPES_, LINN_NARGS(__VA_ARGS__))(value, ##__VA_ARGS__)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_0(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_1(value, t1)     static_cast<t1>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_2(value, t1, t2) static_cast<t1>(value), static_cast<t2>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_3(value, t1, t2, t3) \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_4(value, t1, t2, t3, t4) \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value), static_cast<t4>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_5(value, t1, t2, t3, t4, t5)                                \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value), static_cast<t4>(value), \
        static_cast<t5>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_6(value, t1, t2, t3, t4, t5, t6)                            \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value), static_cast<t4>(value), \
        static_cast<t5>(value), static_cast<t6>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_7(value, t1, t2, t3, t4, t5, t6, t7)                        \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value), static_cast<t4>(value), \
        static_cast<t5>(value), static_cast<t6>(value), static_cast<t7>(value)
#  define Q_SIGNAL_EMIT_ARGS_FROM_TYPES_8(value, t1, t2, t3, t4, t5, t6, t7, t8)                    \
    static_cast<t1>(value), static_cast<t2>(value), static_cast<t3>(value), static_cast<t4>(value), \
        static_cast<t5>(value), static_cast<t6>(value), static_cast<t7>(value), static_cast<t8>(value)

#  define Q_SETTER_EMIT_WITH_PARAMS(NAME, value, ...) \
    Q_EMIT p##NAME##Changed(Q_SIGNAL_EMIT_ARGS_FROM_TYPES(value, Q_SIGNAL_PARAMS_CONTENT_FROM_ARGS(__VA_ARGS__)));
#  define Q_SETTER_EMIT_VOID(NAME, value, ...) Q_EMIT p##NAME##Changed();

#  define Q_SETTER_EMIT_BY_PARAMS_FLAG(flag, NAME, value, ...) \
    LINN_CAT(Q_SETTER_EMIT_BY_PARAMS_FLAG_, flag)(NAME, value, ##__VA_ARGS__)
#  define Q_SETTER_EMIT_BY_PARAMS_FLAG_0(NAME, value, ...) Q_SETTER_EMIT_VOID(NAME, value, ##__VA_ARGS__)
#  define Q_SETTER_EMIT_BY_PARAMS_FLAG_1(NAME, value, ...) Q_SETTER_EMIT_WITH_PARAMS(NAME, value, ##__VA_ARGS__)

#  define Q_SETTER_EMIT_BY_NO_EMIT_FLAG(flag, NAME, value, ...) \
    LINN_CAT(Q_SETTER_EMIT_BY_NO_EMIT_FLAG_, flag)(NAME, value, ##__VA_ARGS__)
#  define Q_SETTER_EMIT_BY_NO_EMIT_FLAG_0(NAME, value, ...) \
    Q_SETTER_EMIT_BY_PARAMS_FLAG(Q_HAS_SIGNAL_PARAMS_FROM_ARGS(__VA_ARGS__), NAME, value, ##__VA_ARGS__)
#  define Q_SETTER_EMIT_BY_NO_EMIT_FLAG_1(NAME, value, ...)

#  define Q_SETTER_EMIT_FROM_ARGS(NAME, value, ...) \
    Q_SETTER_EMIT_BY_NO_EMIT_FLAG(Q_HAS_NO_EMIT_FROM_ARGS(__VA_ARGS__), NAME, value, ##__VA_ARGS__)

#  define Q_CONTRACT_CHECK_ALWAYS_FROM_ARG(NAME, arg) \
    LINN_CAT(Q_CONTRACT_CHECK_ALWAYS_FROM_ARG_, LINN_IS_PAREN(LINN_EXPAND(arg)))(NAME, arg)
#  define Q_CONTRACT_CHECK_ALWAYS_FROM_ARG_0(NAME, arg)
#  define Q_CONTRACT_CHECK_ALWAYS_FROM_ARG_1(NAME, arg)      \
    LINN_IF(Q_IS_TAG_CONTRACT_ALWAYS(LINN_PAIR_FIRST(arg)))( \
        Q_CONTRACT_ASSERT_ALWAYS(Q_UNPAREN(LINN_PAIR_SECOND(arg)));, )

#  define Q_CONTRACT_CHECK_DEBUG_FROM_ARG(NAME, arg) \
    LINN_CAT(Q_CONTRACT_CHECK_DEBUG_FROM_ARG_, LINN_IS_PAREN(LINN_EXPAND(arg)))(NAME, arg)
#  define Q_CONTRACT_CHECK_DEBUG_FROM_ARG_0(NAME, arg)
#  define Q_CONTRACT_CHECK_DEBUG_FROM_ARG_1(NAME, arg) \
    LINN_IF(Q_IS_TAG_CONTRACT_DEBUG(LINN_PAIR_FIRST(arg)))(Q_CONTRACT_ASSERT_DEBUG(Q_UNPAREN(LINN_PAIR_SECOND(arg)));, )

#  define Q_CONTRACT_CHECK_FROM_ARG(NAME, arg)  \
    Q_CONTRACT_CHECK_ALWAYS_FROM_ARG(NAME, arg) \
    Q_CONTRACT_CHECK_DEBUG_FROM_ARG(NAME, arg)

#  define Q_CONTRACT_CHECK_FROM_ARGS(...) LINN_CAT(Q_CONTRACT_CHECK_FROM_ARGS_, LINN_NARGS(__VA_ARGS__))(__VA_ARGS__)
#  define Q_CONTRACT_CHECK_FROM_ARGS_1(NAME)
#  define Q_CONTRACT_CHECK_FROM_ARGS_2(NAME, a1) Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)
#  define Q_CONTRACT_CHECK_FROM_ARGS_3(NAME, a1, a2) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)              \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)
#  define Q_CONTRACT_CHECK_FROM_ARGS_4(NAME, a1, a2, a3) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2) Q_CONTRACT_CHECK_FROM_ARG(NAME, a3)
#  define Q_CONTRACT_CHECK_FROM_ARGS_5(NAME, a1, a2, a3, a4) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a3) Q_CONTRACT_CHECK_FROM_ARG(NAME, a4)
#  define Q_CONTRACT_CHECK_FROM_ARGS_6(NAME, a1, a2, a3, a4, a5) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                          \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)                          \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a3)                          \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a4) Q_CONTRACT_CHECK_FROM_ARG(NAME, a5)
#  define Q_CONTRACT_CHECK_FROM_ARGS_7(NAME, a1, a2, a3, a4, a5, a6) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                              \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)                              \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a3)                              \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a4)                              \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a5) Q_CONTRACT_CHECK_FROM_ARG(NAME, a6)
#  define Q_CONTRACT_CHECK_FROM_ARGS_8(NAME, a1, a2, a3, a4, a5, a6, a7) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)                                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a3)                                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a4)                                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a5)                                  \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a6) Q_CONTRACT_CHECK_FROM_ARG(NAME, a7)
#  define Q_CONTRACT_CHECK_FROM_ARGS_9(NAME, a1, a2, a3, a4, a5, a6, a7, a8) \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a1)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a2)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a3)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a4)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a5)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a6)                                      \
    Q_CONTRACT_CHECK_FROM_ARG(NAME, a7) Q_CONTRACT_CHECK_FROM_ARG(NAME, a8)

#  pragma endregion PROPERTY_HELPER_CORE

#endif // NXPROPERTY_H

#if defined(_MSC_VER)
#  pragma warning(pop)
#endif
