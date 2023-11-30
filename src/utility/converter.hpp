#include <type_traits>

namespace nono {

template <class Head, class... Tail>
class Converter {
    static_assert(std::is_same_v<int, Head>);

  public:
    Converter(Head head, Tail... tail): size_(head), subcomverter_(tail...) {}

    int operator()(Head head, Tail... tail) const {
        return head + size_ * subcomverter_(tail...);
    }

    int size() const {
        return size_ * subcomverter_.size();
    }

  private:
    int size_;
    Converter<Tail...> subcomverter_;
};

template <>
class Converter<int> {
  public:
    Converter(int head): size_(head) {}

    int operator()(int head) const {
        return head;
    }

    int size() const {
        return size_;
    }

  private:
    int size_;
};

}  //  namespace nono
