/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree. An additional grant
 of patent rights can be found in the PATENTS file in the same directory.
 */

#include <gtest/gtest.h>
#include <bom/bom_format.h>
#include <bom/bom.h>
#include <car/car_format.h>
#include <car/Facet.h>
#include <car/AttributeList.h>
#include <car/Rendition.h>
#include <car/Facet.h>
#include <car/Writer.h>
#include <car/Reader.h>

#include <cstdio>
#include <string>

#include <vector>

// Test pattern as raw pixed data, in PremultipliedBGRA8 format
static std::vector<uint8_t> test_pixels = {
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x7f, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x7f, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0xff, 0x00, 0x00, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

struct test_car_key_format {
    struct car_key_format keyfmt;
    uint32_t identifier_list[13];
} __attribute__((packed));

static test_car_key_format keyfmt_s = {
    {
        {'k', 'f', 'm', 't'}, 0, 13,
    },
    {
        car_attribute_identifier_scale,
        car_attribute_identifier_idiom,
        car_attribute_identifier_subtype,
        car_attribute_identifier_graphics_class,
        car_attribute_identifier_memory_class,
        car_attribute_identifier_size_class_horizontal,
        car_attribute_identifier_size_class_vertical,
        car_attribute_identifier_identifier,
        car_attribute_identifier_element,
        car_attribute_identifier_part,
        car_attribute_identifier_state,
        car_attribute_identifier_value,
        car_attribute_identifier_dimension1,
    }
};

static struct car_key_format *keyfmt = &keyfmt_s.keyfmt;

TEST(Writer, TestWriter)
{
    int width = 8;
    int height = 8;

    /* Write out. */
    auto writer_bom = car::Writer::unique_ptr_bom(bom_alloc_empty(bom_context_memory(NULL, 0)), bom_free);
    EXPECT_NE(writer_bom, nullptr);

    auto writer = car::Writer::Create(std::move(writer_bom));
    EXPECT_NE(writer, ext::nullopt);

    writer->keyfmt() = keyfmt;

    car::AttributeList attributes = car::AttributeList({
        { car_attribute_identifier_idiom, car_attribute_identifier_idiom_value_universal },
        { car_attribute_identifier_scale, 2 },
        { car_attribute_identifier_identifier, 1 },
    });

    car::Facet facet = car::Facet::Create("testpattern", attributes);
    writer->addFacet(facet);

    car::Rendition::Data::Format format = car::Rendition::Data::Format::PremultipliedBGRA8;
    auto data = ext::optional<car::Rendition::Data>(car::Rendition::Data(test_pixels, format));
    car::Rendition rendition = car::Rendition::Create(attributes, data);
    rendition.width() = width;
    rendition.height() = height;
    rendition.scale() = 2;
    rendition.fileName() = "testpattern.png";
    rendition.layout() = car_rendition_value_layout_one_part_scale;
    writer->addRendition(rendition);

    writer->write();

    /* Read back. */
    struct bom_context_memory const *writer_memory = bom_memory(writer->bom());
    struct bom_context_memory reader_memory = bom_context_memory(writer_memory->data, writer_memory->size);
    auto reader_bom = std::unique_ptr<struct bom_context, decltype(&bom_free)>(bom_alloc_load(reader_memory), bom_free);
    EXPECT_NE(reader_bom, nullptr);

    ext::optional<car::Reader> reader = car::Reader::Load(std::move(reader_bom));
    EXPECT_NE(reader, ext::nullopt);

    int facet_count = 0;
    int rendition_count = 0;

    reader->facetIterate([&reader, &facet_count, &rendition_count](car::Facet const &facet) {
        facet_count++;

        EXPECT_EQ(facet.name(), "testpattern");

        auto renditions = reader->lookupRenditions(facet);
        for (auto const &rendition : renditions) {
            rendition_count++;

            auto data = rendition.data()->data();
            EXPECT_EQ(data, test_pixels);
        }
    });

    EXPECT_EQ(facet_count, 1);
    EXPECT_EQ(rendition_count, 1);
}

