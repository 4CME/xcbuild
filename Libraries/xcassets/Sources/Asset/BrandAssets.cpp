/**
 Copyright (c) 2015-present, Facebook, Inc.
 All rights reserved.

 This source code is licensed under the BSD-style license found in the
 LICENSE file in the root directory of this source tree.
 */

#include <xcassets/Asset/BrandAssets.h>
#include <plist/Array.h>
#include <plist/String.h>
#include <plist/Keys/Unpack.h>

using xcassets::Asset::BrandAssets;

bool BrandAssets::BrandAsset::
parse(plist::Dictionary const *dict)
{
    std::unordered_set<std::string> seen;
    auto unpack = plist::Keys::Unpack("BrandAssetsBrandAsset", dict, &seen);

    auto F = unpack.cast <plist::String> ("filename");
    auto I = unpack.cast <plist::String> ("idiom");
    auto S = unpack.cast <plist::String> ("size");
    auto R = unpack.cast <plist::String> ("role");

    if (!unpack.complete(true)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (I != nullptr) {
        _idiom = Slot::Idioms::Parse(I->value());
    }

    if (F != nullptr) {
        _fileName = F->value();
    }

    if (S != nullptr) {
        _size = Slot::ImageSize::Parse(S->value());
    }

    if (R != nullptr) {
        _role = BrandAssetRoles::Parse(R->value());
    }

    return true;
}

bool BrandAssets::
parse(plist::Dictionary const *dict, std::unordered_set<std::string> *seen, bool check)
{
    if (!Asset::parse(dict, seen, false)) {
        return false;
    }

    /* Contents is required. */
    if (dict == nullptr) {
        return false;
    }

    auto unpack = plist::Keys::Unpack("BrandAssets", dict, seen);

    auto As = unpack.cast <plist::Array> ("assets");

    if (!unpack.complete(check)) {
        fprintf(stderr, "%s", unpack.errorText().c_str());
    }

    if (As != nullptr) {
        _assets = std::vector<BrandAsset>();

        for (size_t n = 0; n < As->count(); ++n) {
            if (auto dict = As->value<plist::Dictionary>(n)) {
                BrandAsset asset;
                if (asset.parse(dict)) {
                    _assets->push_back(asset);
                }
            }
        }
    }

    return true;
}
