/// brief : monoid template. 使うことなさそう.
template <class T>
struct Monoid {
    using Value = T;
    static Value op(Value lhs, Value rhs);
    static Value e();
};
template <class T>
struct ActMonoid {
    using Value = T;
    using Act = T;
    static Value op(Value lhs, Value rhs);
    static Value e();
    static Value mapping(Act lhs, Value rhs);
    //  rhsは事前に適用されていたやつ
    //  lhsがこれから適用するやつ
    static Act compososition(Act lhs, Act rhs);
    static Act id();
};
template <class U>
struct BeatMonoid {
    struct Value;
    struct Act;
    static Value op(Value lhs, Value rhs);
    static Value e();
    static Value mapping(Act lhs, Value rhs);
    static Act composition(Act lhs, Act rhs);
    static Act id();
};
