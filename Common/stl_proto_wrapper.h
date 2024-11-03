#pragma once

#include <string>
#include <fstream>
#include <filesystem>
#include <google/protobuf/util/message_differencer.h>
#include <google/protobuf/util/json_util.h>

#include "darwin_service.pb.h"
#include "client_parameter.pb.h"

namespace proto {

    // Needed for map and set.
    bool operator<(const proto::Character& lhs, const proto::Character& rhs);

}  // namespace darwin.

namespace darwin {

    template <typename T>
    T LoadProtoFromJson(const std::string& json)
    {
        T proto{};
        google::protobuf::util::JsonParseOptions options{};
        options.ignore_unknown_fields = false;
        auto status =
            google::protobuf::util::JsonStringToMessage(json, &proto, options);
        if (!status.ok())
        {
            throw std::runtime_error(
                "Couldn't parse json status error: " +
                std::string(status.message()));
        }
        return proto;
    }

    template <typename T>
    std::string SaveProtoToJson(const T& proto)
    {
        std::string json;
        google::protobuf::util::JsonPrintOptions options{};
        options.add_whitespace = true;
        options.always_print_primitive_fields = true;
        options.preserve_proto_field_names = true;
        auto status =
            google::protobuf::util::MessageToJsonString(proto, &json, options);
        if (!status.ok())
        {
            throw std::runtime_error(
                "Couldn't parse json status error: " +
                std::string(status.message()));
        }
        return json;
    }

    template <typename T>
    T LoadProtoFromJsonFile(const std::filesystem::path& filename)
    {
        // Empty case (no such file return an empty structure).
        if (filename.empty())
            return T{};
        // Try to open it.
        std::ifstream ifs(filename.string(), std::ios::in);
        if (!ifs.is_open())
        {
            throw std::runtime_error("Couldn't open file: " + filename.string());
        }
        std::string contents(std::istreambuf_iterator<char>(ifs), {});
        return LoadProtoFromJson<T>(contents);
    }

    template <typename T>
    void SaveProtoToJsonFile(
        const T& proto, const
        std::filesystem::path& filename)
    {
        std::string contents = SaveProtoToJson(proto);
        std::ofstream ofs(filename.string(), std::ios::out);
        if (!ofs.is_open())
        {
            throw std::runtime_error(
                "Couldn't open file: " + filename.string());
        }
        ofs << contents;
    }

    bool operator==(
        const proto::Character& character_left, 
        const proto::Character& character_right);

    bool operator==(
        const proto::Element& element_left, 
        const proto::Element& element_right);

    // Helper functions.
    proto::Element CreateBasicElement(
        const std::string& name,
        proto::TypeEnum type_enum,
        proto::Vector3 vector3,
        double mass,
        double radius);
    proto::Character CreateBasicCharacter(
        const std::string& name,
        proto::Vector3 vector3,
        double mass,
        double radius);

} // namespace darwin.