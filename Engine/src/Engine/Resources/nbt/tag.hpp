#pragma once

#include <memory>
#include <map>
#include <vector>

#include "Engine\Resources\nbt\data.hpp"
#include "Engine\Resources\nbt\utils.hpp"
#include "Engine\Resources\nbt\endian.hpp"
#include "Engine\Resources\nbt\io.hpp"

namespace rh::nbt {

    // base tag class
    class tag {
    public:
        //Virtual destructor
        virtual ~tag() noexcept {}

        ///Returns the type of the tag
        virtual tag_type get_type() const noexcept = 0;

        //Polymorphic clone methods
        virtual std::unique_ptr<tag> clone() const& = 0;
        virtual std::unique_ptr<tag> move_clone() && = 0;
        std::unique_ptr<tag> clone()&&;

        // get this generic tag as a specific tag (e.g. tag_compound, tag_byte, etc.)
        template<class T>
        T& as() {
            static_assert(std::is_base_of<tag, T>::value, "T must be a subclass of tag");
            try {
                return dynamic_cast<T&>(*this);
            }
            catch (std::bad_cast e) {
                T __type;
                __debugbreak();
            }
        }
        template<class T>
        const T& as() const {
            static_assert(std::is_base_of<tag, T>::value, "T must be a subclass of tag");
            return dynamic_cast<const T&>(*this);
        }

        //Move-assigns the given tag if the class is the same
        //throw std::bad_cast if rhs is not the same type as *this
        virtual tag& assign(tag&& rhs) = 0;

        // read tag's payload from the stream
        virtual void read_payload(std::istream& is, endian::endian e) = 0;
        // write tag's payload to the stream
        virtual void write_payload(std::ostream& os, endian::endian e) const = 0;

        //Default-constructs a new tag of the given type
        //throw std::invalid_argument if the type is not valid (e.g. End or Null)
        static std::unique_ptr<tag> create(tag_type type);

        friend bool operator==(const tag& lhs, const tag& rhs);
        friend bool operator!=(const tag& lhs, const tag& rhs);

    private:
        virtual bool equals(const tag& rhs) const = 0;
    };

    // generic tag type for primitives(int, float, etc.)
    template <typename T>
    class tag_primitive final : public tag {
    public:
        ///The type of the value
        typedef T value_type;

        ///The type of the tag
        static constexpr tag_type type = utils::get_primitive_type<T>::value;
        tag_type get_type() const noexcept override final { return type; };

        std::unique_ptr<tag> clone() const& override final { return std::make_unique<tag_primitive<T>>(get_this()); }
        std::unique_ptr<tag> move_clone() && override final { return std::make_unique<tag_primitive<T>>(std::move(get_this())); }
        tag& assign(tag&& rhs) override final { return get_this() = dynamic_cast<tag_primitive<T>&&>(rhs); }

        //Constructor
        constexpr tag_primitive(T val = static_cast<T>(0.0)) noexcept : value(val) {}

        //Getters
        operator T& () { return value; }
        constexpr operator T() const { return value; }
        constexpr T get() const { return value; }

        //Setters
        tag_primitive& operator=(T val) { value = val; return *this; }
        void set(T val) { value = val; }

        void read_payload(std::istream& is, endian::endian e) override { io::read_num(is, value, e); }
        void write_payload(std::ostream& os, endian::endian e) const override { io::write_num(os, value, e); }

    private:
        T value;

        bool equals(const tag& rhs) const override final { return get_this() == static_cast<const tag_primitive<T>&>(rhs); }
        tag_primitive<T>& get_this() { return static_cast<tag_primitive<T>&>(*this); }
        const tag_primitive<T>& get_this() const { return static_cast<const tag_primitive<T>&>(*this); }
    };

    //typedefs for easier use
    typedef tag_primitive<nbt_byte> tag_byte;
    typedef tag_primitive<nbt_short> tag_short;
    typedef tag_primitive<nbt_int> tag_int;
    typedef tag_primitive<nbt_long> tag_long;
    typedef tag_primitive<nbt_float> tag_float;
    typedef tag_primitive<nbt_double> tag_double;
    typedef tag_primitive<laml::Vec2> tag_vec2;
    typedef tag_primitive<laml::Vec3> tag_vec3;
    typedef tag_primitive<laml::Vec4> tag_vec4;
    typedef tag_primitive<laml::Mat2> tag_mat2;
    typedef tag_primitive<laml::Mat3> tag_mat3;
    typedef tag_primitive<laml::Mat4> tag_mat4;

    // tag_string
    class tag_string final : public tag {
    public:
        static constexpr tag_type type = tag_type::String;
        tag_type get_type() const noexcept override final { return type; };

        std::unique_ptr<tag> clone() const& override final { return std::make_unique<tag_string>(get_this()); }
        std::unique_ptr<tag> move_clone() && override final { return std::make_unique<tag_string>(std::move(get_this())); }
        tag& assign(tag&& rhs) override final { return get_this() = dynamic_cast<tag_string&&>(rhs); }

        //Constructors
        tag_string() {}
        tag_string(const std::string& str) : value(str) {}
        tag_string(std::string&& str) noexcept : value(std::move(str)) {}
        tag_string(const char* str) : value(str) {}

        //Getters
        operator std::string& () { return value; }
        operator const std::string& () const { return value; }
        const std::string& get() const { return value; }

        //Setters
        tag_string& operator=(const std::string& str) { value = str; return *this; }
        tag_string& operator=(std::string&& str) { value = std::move(str); return *this; }
        tag_string& operator=(const char* str) { value = str; return *this; }
        void set(const std::string& str) { value = str; }
        void set(std::string&& str) { value = std::move(str); }

        void read_payload(std::istream& is, endian::endian e) override;
        void write_payload(std::ostream& os, endian::endian e) const override;

    private:
        std::string value;

        bool equals(const tag& rhs) const override final { return get_this() == static_cast<const tag_string&>(rhs); }
        tag_string& get_this() { return static_cast<tag_string&>(*this); }
        const tag_string& get_this() const { return static_cast<const tag_string&>(*this); }
    };

    // value
    class value
    {
    public:
        //Constructors
        value() noexcept {}
        explicit value(std::unique_ptr<tag>&& t) noexcept : tag_(std::move(t)) {}
        explicit value(tag&& t);

        //Moving
        value(value&&) noexcept = default;
        value& operator=(value&&) noexcept = default;

        //Copying
        explicit value(const value& rhs);
        value& operator=(const value& rhs);

        value& operator=(tag&& t);
        void set(tag&& t);

        //Conversion to tag
        operator tag& () { return get(); }
        operator const tag& () const { return get(); }
        tag& get() { return *tag_; }
        const tag& get() const { return *tag_; }

        template<class T>
        T& as() { return tag_->as<T>(); }
        template<class T>
        const T& as() const { return tag_->as<T>(); }

        //Assignment of primitives and string
        value& operator=(int8_t val);
        value& operator=(int16_t val);
        value& operator=(int32_t val);
        value& operator=(int64_t val);
        value& operator=(float val);
        value& operator=(double val);
        value& operator=(const std::string& str);//TODO: what is this?
        value& operator=(std::string&& str);
        value& operator=(laml::Vec2);
        value& operator=(laml::Vec3);
        value& operator=(laml::Vec4);
        value& operator=(laml::Mat2);
        value& operator=(laml::Mat3);
        value& operator=(laml::Mat4);

        //Conversions to primitives and string
        explicit operator int8_t() const;
        explicit operator int16_t() const;
        explicit operator int32_t() const;
        explicit operator int64_t() const;
        explicit operator float() const;
        explicit operator double() const;
        explicit operator const std::string& () const;
        explicit operator laml::Vec2() const;
        explicit operator laml::Vec3() const;
        explicit operator laml::Vec4() const;
        explicit operator laml::Mat2() const;
        explicit operator laml::Mat3() const;
        explicit operator laml::Mat4() const;

        ///Returns true if the value is not uninitialized
        explicit operator bool() const { return tag_ != nullptr; }

        // access tag by name (compound only) with bounds checking
        value& at(const std::string& key);
        const value& at(const std::string& key) const;
        // access tag by name (compound only)
        value& operator[](const std::string& key);
        value& operator[](const char* key);

        // acces tag by index (list only) with bounds checking
        value& at(size_t i);
        const value& at(size_t i) const;
        // acces tag by index (list only)
        value& operator[](size_t i);
        const value& operator[](size_t i) const;

        //get reference to the base ptr
        std::unique_ptr<tag>& get_ptr() { return tag_; }
        const std::unique_ptr<tag>& get_ptr() const { return tag_; }
        //Resets the underlying std::unique_ptr<tag> to a different value
        void set_ptr(std::unique_ptr<tag>&& t) { tag_ = std::move(t); }

        tag_type get_type() const;

        friend bool operator==(const value& lhs, const value& rhs);
        friend bool operator!=(const value& lhs, const value& rhs);

    private:
        std::unique_ptr<tag> tag_;
    };
    
    // value_init
    class value_initializer final : public value
    {
    public:
        value_initializer(std::unique_ptr<tag>&& t) noexcept : value(std::move(t)) {}
        value_initializer(std::nullptr_t) noexcept : value(nullptr) {}
        value_initializer(value&& val) noexcept : value(std::move(val)) {}
        value_initializer(tag&& t) : value(std::move(t)) {}

        value_initializer(int8_t val);
        value_initializer(int16_t val);
        value_initializer(int32_t val);
        value_initializer(int64_t val);
        value_initializer(float val);
        value_initializer(double val);
        value_initializer(const std::string& str);
        value_initializer(std::string&& str);
        value_initializer(const char* str);
        value_initializer(laml::Vec2 val);
        value_initializer(laml::Vec3 val);
        value_initializer(laml::Vec4 val);
        value_initializer(laml::Mat2 val);
        value_initializer(laml::Mat3 val);
        value_initializer(laml::Mat4 val);
    };

    //tag_compound
    class tag_compound final : public tag {
    public:
        typedef std::map<std::string, value> map_t_;

        //Iterator types
        typedef map_t_::iterator iterator;
        typedef map_t_::const_iterator const_iterator;

        static constexpr tag_type type = tag_type::Compound;
        tag_type get_type() const noexcept override final { return type; };

        std::unique_ptr<tag> clone() const& override final { return std::make_unique<tag_compound>(get_this()); }
        std::unique_ptr<tag> move_clone() && override final { return std::make_unique<tag_compound>(std::move(get_this())); }
        tag& assign(tag&& rhs) override final { return get_this() = dynamic_cast<tag_compound&&>(rhs); }

        //Constructs an empty compound
        tag_compound() {}

        //Constructs a compound with the given key-value pairs
        tag_compound(std::initializer_list<std::pair<std::string, value_initializer>> init);

        // get key (with bounds checking)
        value& at(const std::string& key);
        const value& at(const std::string& key) const;

        // get key (creates if dont exist)
        value& operator[](const std::string& key) { return tags[key]; }

        // Inserts or assigns a tag (creates if not exist)
        std::pair<iterator, bool> put(const std::string& key, value_initializer&& val);

        // Inserts a tag if the key does not exist (does nothing if it does)
        std::pair<iterator, bool> insert(const std::string& key, value_initializer&& val);

        // emplace a new tag
        template<class T, class... Args>
        std::pair<iterator, bool> emplace(const std::string& key, Args&&... args);

        //erases tag from the compound
        bool erase(const std::string& key);

        bool has_key(const std::string& key) const;
        bool has_key(const std::string& key, tag_type type) const;
        size_t size() const { return tags.size(); }

        ///Erases all tags from the compound
        void clear() { tags.clear(); }

        //Iterators
        iterator begin() { return tags.begin(); }
        iterator end() { return tags.end(); }
        const_iterator begin() const { return tags.begin(); }
        const_iterator end() const { return tags.end(); }
        const_iterator cbegin() const { return tags.cbegin(); }
        const_iterator cend() const { return tags.cend(); }

        void read_payload(std::istream& is, endian::endian e) override;
        void write_payload(std::ostream& os, endian::endian e) const override;

        friend bool operator==(const tag_compound& lhs, const tag_compound& rhs)
        {
            return lhs.tags == rhs.tags;
        }
        friend bool operator!=(const tag_compound& lhs, const tag_compound& rhs)
        {
            return !(lhs == rhs);
        }

    private:
        map_t_ tags;

        bool equals(const tag& rhs) const override final { return get_this() == static_cast<const tag_compound&>(rhs); }
        tag_compound& get_this() { return static_cast<tag_compound&>(*this); }
        const tag_compound& get_this() const { return static_cast<const tag_compound&>(*this); }
    };

    // tag_array (base class for byte_array, int_array, etc.)
    template<typename T>
    class tag_array final : public tag {
    public:
        //Iterator types
        typedef typename std::vector<T>::iterator iterator;
        typedef typename std::vector<T>::const_iterator const_iterator;

        //The type of the contained values
        typedef T value_type;

        //The type of the tag
        static constexpr tag_type type = utils::get_array_type<T>::value;
        tag_type get_type() const noexcept override final { return type; };

        std::unique_ptr<tag> clone() const& override final { return std::make_unique<tag_array<T>>(get_this()); }
        std::unique_ptr<tag> move_clone() && override final { return std::make_unique<tag_array<T>>(std::move(get_this())); }
        tag& assign(tag&& rhs) override final { return get_this() = dynamic_cast<tag_array<T>&&>(rhs); }

        //Constructs an empty array
        tag_array() {}

        //Constructs an array with the given values
        tag_array(std::initializer_list<T> init) : data(init) {}
        tag_array(std::vector<T>&& vec) noexcept : data(std::move(vec)) {}

        //Returns a reference to the vector that contains the values
        std::vector<T>& get() { return data; }
        const std::vector<T>& get() const { return data; }

        T& at(size_t i) { return data.at(i); }
        T at(size_t i) const { return data.at(i); }

        T& operator[](size_t i) { return data[i]; }
        T operator[](size_t i) const { return data[i]; }

        //Appends a value at the end of the array
        void push_back(T val) { data.push_back(val); }

        //Removes the last element from the array
        void pop_back() { data.pop_back(); }

        //Returns the number of values in the array
        size_t size() const { return data.size(); }

        //Erases all values from the array.
        void clear() { data.clear(); }

        //Iterators
        iterator begin() { return data.begin(); }
        iterator end() { return data.end(); }
        const_iterator begin() const { return data.begin(); }
        const_iterator end() const { return data.end(); }
        const_iterator cbegin() const { return data.cbegin(); }
        const_iterator cend() const { return data.cend(); }

        void read_payload(std::istream& is, endian::endian e) override;
        void write_payload(std::ostream& os, endian::endian e) const override;

    private:
        std::vector<T> data;

        bool equals(const tag& rhs) const override final { return get_this() == static_cast<const tag_array<T>&>(rhs); }
        tag_array<T>& get_this() { return static_cast<tag_array<T>&>(*this); }
        const tag_array<T>& get_this() const { return static_cast<const tag_array<T>&>(*this); }
    };

    //typedefs for easier use
    typedef tag_array<nbt_byte> tag_byte_array;
    typedef tag_array<nbt_int>  tag_int_array;
    typedef tag_array<nbt_long> tag_long_array;

    // tag_list
    class tag_list final : public tag {
    public:
        //Iterator types
        typedef std::vector<value>::iterator iterator;
        typedef std::vector<value>::const_iterator const_iterator;

        //The type of the tag
        static constexpr tag_type type = tag_type::List;
        tag_type get_type() const noexcept override final { return type; };

        std::unique_ptr<tag> clone() const& override final { return std::make_unique<tag_list>(get_this()); }
        std::unique_ptr<tag> move_clone() && override final { return std::make_unique<tag_list>(std::move(get_this())); }
        tag& assign(tag&& rhs) override final { return get_this() = dynamic_cast<tag_list&&>(rhs); }

        // Constructs a list of type T with the given values
        template<class T>
        static tag_list of(std::initializer_list<T> init) {
            tag_list result;
            result.init<T, T>(init);
            return result;
        }

        //make empty list
        tag_list() : tag_list(tag_type::Null) {}

        //Constructs an empty list with the given content type
        explicit tag_list(tag_type type) : el_type_(type) {}

        ///Constructs a list with the given contents
        tag_list(std::initializer_list<int8_t> init);
        tag_list(std::initializer_list<int16_t> init);
        tag_list(std::initializer_list<int32_t> init);
        tag_list(std::initializer_list<int64_t> init);
        tag_list(std::initializer_list<float> init);
        tag_list(std::initializer_list<double> init);
        tag_list(std::initializer_list<std::string> init);
        tag_list(std::initializer_list<tag_byte_array> init);
        tag_list(std::initializer_list<tag_list> init);
        tag_list(std::initializer_list<tag_compound> init);
        tag_list(std::initializer_list<tag_int_array> init);
        tag_list(std::initializer_list<tag_long_array> init);
        tag_list(std::initializer_list<value> init);
        tag_list(std::initializer_list<laml::Vec2> init);
        tag_list(std::initializer_list<laml::Vec3> init);
        tag_list(std::initializer_list<laml::Vec4> init);
        tag_list(std::initializer_list<laml::Mat2> init);
        tag_list(std::initializer_list<laml::Mat3> init);
        tag_list(std::initializer_list<laml::Mat4> init);

        // get tag by index with bounds checking
        value& at(size_t i);
        const value& at(size_t i) const;

        //get tag by index
        value& operator[](size_t i) { return tags[i]; }
        const value& operator[](size_t i) const { return tags[i]; }

        //assign value
        void set(size_t i, value&& val);

        // appedn to end
        void push_back(value_initializer&& val);

        // construct new element and append
        template<class T, class... Args>
        void emplace_back(Args&&... args) {
            if (el_type_ == tag_type::Null) //set content type if undetermined
                el_type_ = T::type;
            else if (el_type_ != T::type)
                throw std::invalid_argument("The tag type does not match the list's content type");
            tags.emplace_back(make_unique<T>(std::forward<Args>(args)...));
        }

        //Removes the last element of the list
        void pop_back() { tags.pop_back(); }

        //Returns the content type of the list, or tag_type::Null if undetermined
        tag_type el_type() const { return el_type_; }

        //Returns the number of tags in the list
        size_t size() const { return tags.size(); }

        //Erases all tags from the list. Preserves the content type.
        void clear() { tags.clear(); }

        // erase list and reset content type
        void reset(tag_type type = tag_type::Null);

        //Iterators
        iterator begin() { return tags.begin(); }
        iterator end() { return tags.end(); }
        const_iterator begin() const { return tags.begin(); }
        const_iterator end() const { return tags.end(); }
        const_iterator cbegin() const { return tags.cbegin(); }
        const_iterator cend() const { return tags.cend(); }

        void read_payload(std::istream& is, endian::endian e) override;
        void write_payload(std::ostream& os, endian::endian e) const override;

        friend bool operator==(const tag_list& lhs, const tag_list& rhs);
        friend bool operator!=(const tag_list& lhs, const tag_list& rhs);

    private:
        std::vector<value> tags;
        tag_type el_type_; // tag_type in the list

        template<class T, class Arg>
        void init(std::initializer_list<Arg> il) {
            el_type_ = T::type;
            tags.reserve(il.size());
            for (const Arg& arg : il)
                tags.emplace_back(std::make_unique<T>(arg));
        }

        bool equals(const tag& rhs) const override final { return get_this() == static_cast<const tag_list&>(rhs); }
        tag_list& get_this() { return static_cast<tag_list&>(*this); }
        const tag_list& get_this() const { return static_cast<const tag_list&>(*this); }
    };

    //Output operator for tag types
    std::ostream& operator<<(std::ostream& os, tag_type tt);
    std::ostream& operator<<(std::ostream& os, const tag& t);

    // equality functions
    template<class T> bool operator==(const tag_primitive<T>& lhs, const tag_primitive<T>& rhs)
    {
        return (lhs.get() == rhs.get());
    }
    template<class T> bool operator!=(const tag_primitive<T>& lhs, const tag_primitive<T>& rhs)
    {
        return !(lhs.get() == rhs.get());
    }

    template<class T> bool operator==(const tag_array<T>& lhs, const tag_array<T>& rhs)
    {
        return (lhs.get() == rhs.get());
    }
    template<class T> bool operator!=(const tag_array<T>& lhs, const tag_array<T>& rhs)
    {
        return !(lhs.get() == rhs.get());
    }
}