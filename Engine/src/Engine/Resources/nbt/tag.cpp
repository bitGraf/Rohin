#include <enpch.hpp>
#include "Engine/Resources/nbt/tag.hpp"
#include <ostream>
#include <stdexcept>
#include <assert.h>

namespace nbt
{

    static_assert(std::numeric_limits<float>::is_iec559&& std::numeric_limits<double>::is_iec559,
        "The floating point values for NBT must conform to IEC 559/IEEE 754");

    bool is_valid_type(int type, bool allow_end) {
        return (allow_end ? 0 : 1) <= type && type <= 12;
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * tag * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    std::unique_ptr<tag> tag::clone()&&{
        return std::move(*this).move_clone();
    }

    std::unique_ptr<tag> tag::create(tag_type type) {
        switch (type) {
        case tag_type::Byte:        return std::make_unique<tag_byte>();
        case tag_type::Short:       return std::make_unique<tag_short>();
        case tag_type::Int:         return std::make_unique<tag_int>();
        case tag_type::Long:        return std::make_unique<tag_long>();
        case tag_type::Float:       return std::make_unique<tag_float>();
        case tag_type::Double:      return std::make_unique<tag_double>();
        case tag_type::Byte_Array:  return std::make_unique<tag_byte_array>();
        case tag_type::String:      return std::make_unique<tag_string>();
        case tag_type::List:        return std::make_unique<tag_list>();
        case tag_type::Compound:    return std::make_unique<tag_compound>();
        case tag_type::Int_Array:   return std::make_unique<tag_int_array>();
        case tag_type::Long_Array:  return std::make_unique<tag_long_array>();
        case tag_type::Vector2:     return std::make_unique<tag_vec2>();
        case tag_type::Vector3:     return std::make_unique<tag_vec3>();
        case tag_type::Vector4:     return std::make_unique<tag_vec4>();
        case tag_type::Matrix2:     return std::make_unique<tag_mat2>();
        case tag_type::Matrix3:     return std::make_unique<tag_mat3>();
        case tag_type::Matrix4:     return std::make_unique<tag_mat4>();

        default: throw std::invalid_argument("Invalid tag type");
        }
    }

    bool operator==(const tag& lhs, const tag& rhs) {
        if (typeid(lhs) != typeid(rhs))
            return false;
        return lhs.equals(rhs);
    }

    bool operator!=(const tag& lhs, const tag& rhs) {
        return !(lhs == rhs);
    }

    std::ostream& operator<<(std::ostream& os, tag_type tt) {
        switch (tt) {
        case tag_type::End:         return os << "end";
        case tag_type::Byte:        return os << "byte";
        case tag_type::Short:       return os << "short";
        case tag_type::Int:         return os << "int";
        case tag_type::Long:        return os << "long";
        case tag_type::Float:       return os << "float";
        case tag_type::Double:      return os << "double";
        case tag_type::Byte_Array:  return os << "byte_array";
        case tag_type::String:      return os << "string";
        case tag_type::List:        return os << "list";
        case tag_type::Compound:    return os << "compound";
        case tag_type::Int_Array:   return os << "int_array";
        case tag_type::Long_Array:  return os << "long_array";
        case tag_type::Vector2:     return os << "vector2";
        case tag_type::Vector3:     return os << "vector3";
        case tag_type::Vector4:     return os << "vector4";
        case tag_type::Matrix2:     return os << "matrix2";
        case tag_type::Matrix3:     return os << "matrix3";
        case tag_type::Matrix4:     return os << "matrix4";
        case tag_type::Null:        return os << "null";

        default:                    return os << "invalid";
        }
    }

    std::string printSpaces(int deeper = 0) {
        static std::uint8_t depth = 0;
        if (deeper>0) depth++;
        if (deeper<0) depth--;
        //if (reset) depth = 0;
        if (depth < 0) depth = 0;

        if (depth > 0) {
            std::string text(depth * 2, ' ');
            //std::string text(depth * 2 - 1, '\xC4');
            //text.insert(text.begin(), '\xC3');
            return text;
        }
        else
            return "";
    }

    std::ostream& operator<<(std::ostream& os, const tag& t) {
        //might want to output the tag info to a stream for debug purposes
        switch (t.get_type()) {
        case tag_type::End:         return os << "tag_end";
        case tag_type::Byte:        return os << (short)t.as<tag_byte>().get();
        case tag_type::Short:       return os << t.as<tag_short>().get();
        case tag_type::Int:         return os << t.as<tag_int>().get();
        case tag_type::Long:        return os << t.as<tag_long>().get();
        case tag_type::Float:       return os << t.as<tag_float>().get();
        case tag_type::Double:      return os << t.as<tag_double>().get();
        case tag_type::Vector2:     return os << t.as<tag_vec2>().get();
        case tag_type::Vector3:     return os << t.as<tag_vec3>().get();
        case tag_type::Vector4:     return os << t.as<tag_vec4>().get();
        case tag_type::Matrix2:     return os << t.as<tag_mat2>().get();
        case tag_type::Matrix3:     return os << t.as<tag_mat3>().get();
        case tag_type::Matrix4:     return os << t.as<tag_mat4>().get();
        case tag_type::Byte_Array: {
            const auto& arr = t.as<tag_byte_array>();
            os << arr.size() << " elements {";
            auto numElem = arr.size() < io::max_array_print ? arr.size() : io::max_array_print;
            for (int n = 0; n < numElem; n++) {
                os << (int)arr[n] << ", ";
                if (n == numElem - 1)
                    os << "...";
            }
            os << "}";
            return os;
        }
        case tag_type::String:      return os << t.as<tag_string>().get();
        case tag_type::List: {
            const auto& list = t.as<tag_list>();
            os << list.size() << " elements {";
            auto numElem = list.size() < io::max_array_print ? list.size() : io::max_array_print;
            for (int n = 0; n < numElem; n++) {
                os << list[n];
                if (n < numElem - 1)
                    os << ", ";
                if (n == numElem - 1 && numElem != list.size())
                    os << "...";
            }
            os << "}";
            return os;
        }
        case tag_type::Compound: {
            os << std::endl;
            printSpaces(1);
            for (auto& tag : t.as<tag_compound>()) {
                os << printSpaces() << "tag[" << tag.second.get_type() << "] \"";
                if (tag.first.size() > io::max_name_len) {
                    os << std::string(tag.first, 0, io::max_name_len) << "... ";
                }
                else {
                    os << tag.first;
                }
                os << "\" = " << tag.second.get() << std::endl;
            }
            os << printSpaces() << "tag[end]";
            printSpaces(-1);
            return os;
        }
        case tag_type::Int_Array: {
            const auto& arr = t.as<tag_int_array>();
            os << arr.size() << " elements {";
            auto numElem = arr.size() < io::max_array_print ? arr.size() : io::max_array_print;
            for (int n = 0; n < numElem; n++) {
                os << arr[n];
                if (n < numElem-1)
                    os << ", ";
                if (n == numElem - 1 && numElem != arr.size())
                    os << "...";
            }
            os << "}";
            return os;
        }
        case tag_type::Long_Array: {
            const auto& arr = t.as<tag_long_array>();
            os << arr.size() << " elements {";
            auto numElem = arr.size() < io::max_array_print ? arr.size() : io::max_array_print;
            for (int n = 0; n < numElem; n++) {
                os << arr[n];
                if (n < numElem - 1)
                    os << ", ";
                if (n == numElem - 1 && numElem != arr.size())
                    os << "...";
            }
            os << "}";
            return os;
        }
        case tag_type::Null:        return os << "null";
        default:                    return os << "invalid";
        }
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * tag_string  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    void tag_string::read_payload(std::istream& is, endian::endian e) {
        value = io::read_string(is, e);
    }

    void tag_string::write_payload(std::ostream& os, endian::endian e) const {
        io::write_string(os, value, e);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * tag_compound  * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    tag_compound::tag_compound(std::initializer_list<std::pair<std::string, value_initializer>> init) {
        for (const auto& pair : init)
            tags.emplace(std::move(pair.first), std::move(pair.second));
    }

    value& tag_compound::at(const std::string& key) {
        return tags.at(key);
    }

    const value& tag_compound::at(const std::string& key) const {
        return tags.at(key);
    }

    std::pair<tag_compound::iterator, bool> tag_compound::put(const std::string& key, value_initializer&& val) {
        auto it = tags.find(key);
        if (it != tags.end()) {
            it->second = std::move(val);
            return { it, false };
        } else {
            return tags.emplace(key, std::move(val));
        }
    }

    std::pair<tag_compound::iterator, bool> tag_compound::insert(const std::string& key, value_initializer&& val) {
        return tags.emplace(key, std::move(val));
    }

    bool tag_compound::erase(const std::string& key) {
        return tags.erase(key) != 0;
    }

    bool tag_compound::has_key(const std::string& key) const {
        return tags.find(key) != tags.end();
    }

    bool tag_compound::has_key(const std::string& key, tag_type type) const {
        auto it = tags.find(key);
        return it != tags.end() && it->second.get_type() == type;
    }

    void tag_compound::read_payload(std::istream& is, endian::endian e) {
        clear();
        tag_type tt;
        while ((tt = io::read_type(is)) != tag_type::End) {
            std::string key;
            try {
                key = io::read_string(is, e);
            } catch (std::exception e) {
                std::cout << "Failed to read type: " << tt << std::endl;
            }
            auto tptr = io::read_payload(is, tt, e);
            tags.emplace(std::move(key), value(std::move(tptr)));
        }
    }

    void tag_compound::write_payload(std::ostream& os, endian::endian e) const {
        for (const auto& pair : tags)
            io::write_tag(os, pair.first, pair.second, e);
        io::write_type(os, tag_type::End);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * value * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    value::value(tag&& t) :
        tag_(std::move(t).move_clone())
    {}

    value::value(const value& rhs) :
        tag_(rhs.tag_ ? rhs.tag_->clone() : nullptr)
    {}

    value& value::operator=(const value& rhs) {
        if (this != &rhs) {
            tag_ = rhs.tag_ ? rhs.tag_->clone() : nullptr;
        }
        return *this;
    }

    value& value::operator=(tag&& t) {
        set(std::move(t));
        return *this;
    }

    void value::set(tag&& t) {
        if (tag_)
            tag_->assign(std::move(t));
        else
            tag_ = std::move(t).move_clone();
    }

    //Primitive assignment
    //TODO: Make this less copy-paste
    value& value::operator=(int8_t val) {
        if (!tag_)
            set(tag_byte(val));
        else switch (tag_->get_type()) {
        case tag_type::Byte:
            static_cast<tag_byte&>(*tag_).set(val);
            break;
        case tag_type::Short:
            static_cast<tag_short&>(*tag_).set(val);
            break;
        case tag_type::Int:
            static_cast<tag_int&>(*tag_).set(val);
            break;
        case tag_type::Long:
            static_cast<tag_long&>(*tag_).set(val);
            break;
        case tag_type::Float:
            static_cast<tag_float&>(*tag_).set(val);
            break;
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(int16_t val) {
        if (!tag_)
            set(tag_short(val));
        else switch (tag_->get_type()) {
        case tag_type::Short:
            static_cast<tag_short&>(*tag_).set(val);
            break;
        case tag_type::Int:
            static_cast<tag_int&>(*tag_).set(val);
            break;
        case tag_type::Long:
            static_cast<tag_long&>(*tag_).set(val);
            break;
        case tag_type::Float:
            static_cast<tag_float&>(*tag_).set(val);
            break;
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(int32_t val) {
        if (!tag_)
            set(tag_int(val));
        else switch (tag_->get_type()) {
        case tag_type::Int:
            static_cast<tag_int&>(*tag_).set(val);
            break;
        case tag_type::Long:
            static_cast<tag_long&>(*tag_).set(val);
            break;
        case tag_type::Float:
            static_cast<tag_float&>(*tag_).set(val);
            break;
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(int64_t val) {
        if (!tag_)
            set(tag_long(val));
        else switch (tag_->get_type()) {
        case tag_type::Long:
            static_cast<tag_long&>(*tag_).set(val);
            break;
        case tag_type::Float:
            static_cast<tag_float&>(*tag_).set(val);
            break;
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(float val) {
        if (!tag_)
            set(tag_float(val));
        else switch (tag_->get_type()) {
        case tag_type::Float:
            static_cast<tag_float&>(*tag_).set(val);
            break;
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(double val) {
        if (!tag_)
            set(tag_double(val));
        else switch (tag_->get_type()) {
        case tag_type::Double:
            static_cast<tag_double&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::vec2 val) {
        if (!tag_)
            set(tag_vec2(val));
        else switch (tag_->get_type()) {
        case tag_type::Vector2:
            static_cast<tag_vec2&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::vec3 val) {
        if (!tag_)
            set(tag_vec3(val));
        else switch (tag_->get_type()) {
        case tag_type::Vector3:
            static_cast<tag_vec3&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::vec4 val) {
        if (!tag_)
            set(tag_vec4(val));
        else switch (tag_->get_type()) {
        case tag_type::Vector4:
            static_cast<tag_vec4&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::mat2 val) {
        if (!tag_)
            set(tag_mat2(val));
        else switch (tag_->get_type()) {
        case tag_type::Matrix2:
            static_cast<tag_mat2&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::mat3 val) {
        if (!tag_)
            set(tag_mat3(val));
        else switch (tag_->get_type()) {
        case tag_type::Matrix3:
            static_cast<tag_mat3&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    value& value::operator=(math::mat4 val) {
        if (!tag_)
            set(tag_mat4(val));
        else switch (tag_->get_type()) {
        case tag_type::Matrix4:
            static_cast<tag_mat4&>(*tag_).set(val);
            break;

        default:
            throw std::bad_cast();
        }
        return *this;
    }

    //Primitive conversion
    value::operator int8_t() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator int16_t() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();
        case tag_type::Short:
            return static_cast<tag_short&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator int32_t() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();
        case tag_type::Short:
            return static_cast<tag_short&>(*tag_).get();
        case tag_type::Int:
            return static_cast<tag_int&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator int64_t() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();
        case tag_type::Short:
            return static_cast<tag_short&>(*tag_).get();
        case tag_type::Int:
            return static_cast<tag_int&>(*tag_).get();
        case tag_type::Long:
            return static_cast<tag_long&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator float() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();
        case tag_type::Short:
            return static_cast<tag_short&>(*tag_).get();
        case tag_type::Int:
            return static_cast<tag_int&>(*tag_).get();
        case tag_type::Long:
            return static_cast<tag_long&>(*tag_).get();
        case tag_type::Float:
            return static_cast<tag_float&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator double() const {
        switch (tag_->get_type()) {
        case tag_type::Byte:
            return static_cast<tag_byte&>(*tag_).get();
        case tag_type::Short:
            return static_cast<tag_short&>(*tag_).get();
        case tag_type::Int:
            return static_cast<tag_int&>(*tag_).get();
        case tag_type::Long:
            return static_cast<tag_long&>(*tag_).get();
        case tag_type::Float:
            return static_cast<tag_float&>(*tag_).get();
        case tag_type::Double:
            return static_cast<tag_double&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::vec2() const {
        switch (tag_->get_type()) {
        case tag_type::Vector2:
            return static_cast<tag_vec2&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::vec3() const {
        switch (tag_->get_type()) {
        case tag_type::Vector3:
            return static_cast<tag_vec3&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::vec4() const {
        switch (tag_->get_type()) {
        case tag_type::Vector4:
            return static_cast<tag_vec4&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::mat2() const {
        switch (tag_->get_type()) {
        case tag_type::Matrix2:
            return static_cast<tag_mat2&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::mat3() const {
        switch (tag_->get_type()) {
        case tag_type::Matrix3:
            return static_cast<tag_mat3&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value::operator math::mat4() const {
        switch (tag_->get_type()) {
        case tag_type::Matrix4:
            return static_cast<tag_mat4&>(*tag_).get();

        default:
            throw std::bad_cast();
        }
    }

    value& value::operator=(std::string&& str) {
        if (!tag_)
            set(tag_string(std::move(str)));
        else
            dynamic_cast<tag_string&>(*tag_).set(std::move(str));
        return *this;
    }

    value::operator const std::string& () const {
        return dynamic_cast<tag_string&>(*tag_).get();
    }

    value& value::at(const std::string& key) {
        return dynamic_cast<tag_compound&>(*tag_).at(key);
    }

    const value& value::at(const std::string& key) const {
        return dynamic_cast<const tag_compound&>(*tag_).at(key);
    }

    value& value::operator[](const std::string& key) {
        return dynamic_cast<tag_compound&>(*tag_)[key];
    }

    value& value::operator[](const char* key) {
        return (*this)[std::string(key)];
    }

    value& value::at(size_t i) {
        return dynamic_cast<tag_list&>(*tag_).at(i);
    }

    const value& value::at(size_t i) const {
        return dynamic_cast<const tag_list&>(*tag_).at(i);
    }

    value& value::operator[](size_t i) {
        return dynamic_cast<tag_list&>(*tag_)[i];
    }

    const value& value::operator[](size_t i) const {
        return dynamic_cast<const tag_list&>(*tag_)[i];
    }

    tag_type value::get_type() const {
        return tag_ ? tag_->get_type() : tag_type::Null;
    }

    bool operator==(const value& lhs, const value& rhs) {
        if (lhs.tag_ != nullptr && rhs.tag_ != nullptr)
            return *lhs.tag_ == *rhs.tag_;
        else
            return lhs.tag_ == nullptr && rhs.tag_ == nullptr;
    }

    bool operator!=(const value& lhs, const value& rhs) {
        return !(lhs == rhs);
    }

    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * value_initializer * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
    value_initializer::value_initializer(int8_t val) : value(tag_byte(val)) {}
    value_initializer::value_initializer(int16_t val) : value(tag_short(val)) {}
    value_initializer::value_initializer(int32_t val) : value(tag_int(val)) {}
    value_initializer::value_initializer(int64_t val) : value(tag_long(val)) {}
    value_initializer::value_initializer(float val) : value(tag_float(val)) {}
    value_initializer::value_initializer(double val) : value(tag_double(val)) {}
    value_initializer::value_initializer(const std::string& str) : value(tag_string(str)) {}
    value_initializer::value_initializer(std::string&& str) : value(tag_string(std::move(str))) {}
    value_initializer::value_initializer(const char* str) : value(tag_string(str)) {}
    value_initializer::value_initializer(math::vec2 val) : value(tag_vec2(val)) {}
    value_initializer::value_initializer(math::vec3 val) : value(tag_vec3(val)) {}
    value_initializer::value_initializer(math::vec4 val) : value(tag_vec4(val)) {}
    value_initializer::value_initializer(math::mat2 val) : value(tag_mat2(val)) {}
    value_initializer::value_initializer(math::mat3 val) : value(tag_mat3(val)) {}
    value_initializer::value_initializer(math::mat4 val) : value(tag_mat4(val)) {}


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * tag_list  * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    tag_list::tag_list(std::initializer_list<int8_t>         il) { init<tag_byte>(il); }
    tag_list::tag_list(std::initializer_list<int16_t>        il) { init<tag_short>(il); }
    tag_list::tag_list(std::initializer_list<int32_t>        il) { init<tag_int>(il); }
    tag_list::tag_list(std::initializer_list<int64_t>        il) { init<tag_long>(il); }
    tag_list::tag_list(std::initializer_list<float>          il) { init<tag_float>(il); }
    tag_list::tag_list(std::initializer_list<double>         il) { init<tag_double>(il); }
    tag_list::tag_list(std::initializer_list<std::string>    il) { init<tag_string>(il); }
    tag_list::tag_list(std::initializer_list<tag_byte_array> il) { init<tag_byte_array>(il); }
    tag_list::tag_list(std::initializer_list<tag_list>       il) { init<tag_list>(il); }
    tag_list::tag_list(std::initializer_list<tag_compound>   il) { init<tag_compound>(il); }
    tag_list::tag_list(std::initializer_list<tag_int_array>  il) { init<tag_int_array>(il); }
    tag_list::tag_list(std::initializer_list<tag_long_array> il) { init<tag_long_array>(il); }
    tag_list::tag_list(std::initializer_list<math::vec2>     il) { init<tag_vec2>(il); }
    tag_list::tag_list(std::initializer_list<math::vec3>     il) { init<tag_vec3>(il); }
    tag_list::tag_list(std::initializer_list<math::vec4>     il) { init<tag_vec4>(il); }
    tag_list::tag_list(std::initializer_list<math::mat2>     il) { init<tag_mat2>(il); }
    tag_list::tag_list(std::initializer_list<math::mat3>     il) { init<tag_mat3>(il); }
    tag_list::tag_list(std::initializer_list<math::mat4>     il) { init<tag_mat4>(il); }

    tag_list::tag_list(std::initializer_list<value> init) {
        if (init.size() == 0)
            el_type_ = tag_type::Null;
        else {
            el_type_ = init.begin()->get_type();
            for (const value& val : init) {
                if (!val || val.get_type() != el_type_)
                    throw std::invalid_argument("The values are not all the same type");
            }
            tags.assign(init.begin(), init.end());
        }
    }

    value& tag_list::at(size_t i) {
        return tags.at(i);
    }

    const value& tag_list::at(size_t i) const {
        return tags.at(i);
    }

    void tag_list::set(size_t i, value&& val) {
        if (val.get_type() != el_type_)
            throw std::invalid_argument("The tag type does not match the list's content type");
        tags.at(i) = std::move(val);
    }

    void tag_list::push_back(value_initializer&& val) {
        if (!val) //don't allow null values
            throw std::invalid_argument("The value must not be null");
        if (el_type_ == tag_type::Null) //set content type if undetermined
            el_type_ = val.get_type();
        else if (el_type_ != val.get_type())
            throw std::invalid_argument("The tag type does not match the list's content type");
        tags.push_back(std::move(val));
    }

    void tag_list::reset(tag_type type) {
        clear();
        el_type_ = type;
    }

    void tag_list::read_payload(std::istream& is, endian::endian e) {
        tag_type lt = io::read_type(is);

        int32_t length;
        io::read_num(is, length, e);
        if (length < 0)
            is.setstate(std::ios::failbit);
        if (!is)
            __debugbreak();

        if (lt != tag_type::End) {
            reset(lt);
            tags.reserve(length);

            for (int32_t i = 0; i < length; ++i)
                tags.emplace_back(io::read_payload(is, lt, e));
        } else {
            //if tag_end, ignore the length, and leave type undetermined
            reset(tag_type::Null);
        }
    }

    void tag_list::write_payload(std::ostream& os, endian::endian e) const {
        if (size() > io::max_array_len) {
            os.setstate(std::ios::failbit);
            throw std::length_error("List is too large for NBT");
        }
        io::write_type(os, el_type_ != tag_type::Null
            ? el_type_
            : tag_type::End);
        nbt_int _size = size();
        io::write_num(os, _size, e);
        for (const auto& val : tags) {
            //check if the value is of the correct type
            if (val.get_type() != el_type_) {
                os.setstate(std::ios::failbit);
                throw std::logic_error("The tags in the list do not all match the content type");
            }
            io::write_payload(os, val, e);
        }
    }

    bool operator==(const tag_list& lhs, const tag_list& rhs) {
        return lhs.el_type_ == rhs.el_type_ && lhs.tags == rhs.tags;
    }

    bool operator!=(const tag_list& lhs, const tag_list& rhs) {
        return !(lhs == rhs);
    }


    /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * tag_array  * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
     * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */

    //manually write byte function
    template<>
    void tag_array<int8_t>::read_payload(std::istream& is, endian::endian e) {
        nbt_int length;
        io::read_num(is, length, e);
        if (length < 0)
            is.setstate(std::ios::failbit);
        if (!is)
            __debugbreak();

        data.resize(length);
        is.read(reinterpret_cast<char*>(data.data()), length);
        if (!is)
            __debugbreak();
    }

    template<typename T>
    void tag_array<T>::read_payload(std::istream& is, endian::endian e) {
        nbt_int length;
        io::read_num(is, length, e);
        if (length < 0)
            is.setstate(std::ios::failbit);
        if (!is)
            __debugbreak();

        data.clear();
        data.reserve(length);
        for (int32_t i = 0; i < length; ++i) {
            T val;
            io::read_num(is, val, e);
            data.push_back(val);
        }
        if (!is)
            __debugbreak();
    }

    template<>
    void tag_array<nbt_byte>::write_payload(std::ostream& os, endian::endian e) const {
        if (size() > io::max_array_len) {
            os.setstate(std::ios::failbit);
            throw std::length_error("Byte array is too large for NBT");
        }
        nbt_int _size = size();
        io::write_num(os, _size, e);
        os.write(reinterpret_cast<const char*>(data.data()), data.size());
    }

    template<typename T>
    void tag_array<T>::write_payload(std::ostream& os, endian::endian e) const {
        if (size() > io::max_array_len) {
            os.setstate(std::ios::failbit);
            throw std::length_error("Array is too large for NBT");
        }
        nbt_int _size = size();
        io::write_num(os, _size, e);
        for (T i : data)
            io::write_num(os, i, e);
    }
}