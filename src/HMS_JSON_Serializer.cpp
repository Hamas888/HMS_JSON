#include "HMS_JSON_Serializer.h"

namespace HMS {

    std::string JsonSerializer::toString(const JsonValue& v, bool pretty, int indent) {
        std::ostringstream oss;
        serializeInternal(v, oss, pretty, indent, 0);
        return oss.str();
    }

    void JsonSerializer::serialize(const JsonValue& v, std::ostream& out, bool pretty, int indent) {
        serializeInternal(v, out, pretty, indent, 0);
    }

    std::string JsonSerializer::escape(const std::string& s) {
        std::string out; out.reserve(s.size());
        for (char c : s) {
            switch (c) {
                case '\"': out += "\\\""; break;
                case '\\': out += "\\\\"; break;
                case '\b': out += "\\b"; break;
                case '\f': out += "\\f"; break;
                case '\n': out += "\\n"; break;
                case '\r': out += "\\r"; break;
                case '\t': out += "\\t"; break;
                default: out.push_back(c); break;
            }
        }
        return out;
    }

    void JsonSerializer::serializeInternal(const JsonValue& v, std::ostream& out, bool pretty, int indent, int level) {
        if (v.isNull()) { out << "null"; return; }
        if (v.isBool()) { out << (v.asBool() ? "true" : "false"); return; }
        if (v.isNumber()) {
            double d = v.asNumber();
            if (std::isfinite(d)) out << d;
            else out << "null";
            return;
        }

        if (v.isString()) { out << '\"' << escape(v.asString()) << '\"'; return; }

        if (v.isArray()) {
            const auto &a = v.asArray();
            out << '[';
            if (pretty && !a.empty()) out << '\n';
            for (size_t i=0;i<a.size();++i) {
                if (pretty) out << std::string(static_cast<size_t>((level+1)*indent), ' ');
                serializeInternal(a[i], out, pretty, indent, level+1);
                if (i+1 < a.size()) out << (pretty ? ",\n" : ",");
            }
            if (pretty && !a.empty()) out << '\n' << std::string(static_cast<size_t>(level*indent), ' ');
            out << ']';
            return;
        }

        if (v.isObject()) {
            const auto &o = v.asObject();
            out << '{';
            if (pretty && !o.empty()) out << '\n';
            size_t idx=0;
            for (auto &kv : o) {
                if (pretty) out << std::string(static_cast<size_t>((level+1)*indent), ' ');
                out << '\"' << escape(kv.first) << '\"' << (pretty ? ": " : ":");
                serializeInternal(kv.second, out, pretty, indent, level+1);
                if (++idx < o.size()) out << (pretty ? ",\n" : ",");
            }
            if (pretty && !o.empty()) out << '\n' << std::string(static_cast<size_t>(level*indent), ' ');
            out << '}';
            return;
        }
    }
}
