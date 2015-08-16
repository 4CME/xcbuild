// Copyright 2013-present Facebook. All Rights Reserved.

#include <pbxproj/XC/Scheme.h>
#include <pbxproj/XC/Actions.h>

using pbxproj::XC::Scheme;
using libutil::FSUtil;

Scheme::Scheme(std::string const &name, std::string const &owner) :
    _name              (name),
    _owner             (owner),
    _lastUpgradeVersion(0)
{
}

bool Scheme::
parse(plist::Dictionary const *dict)
{
    auto S = dict->value <plist::Dictionary> ("Scheme");
    if (S == nullptr)
        return false;

    auto LUV = S->value <plist::Integer> ("LastUpgradeVersion");
    auto V   = S->value <plist::String> ("version");
    auto TA  = S->value <plist::Dictionary> ("TestAction");
    auto LA  = S->value <plist::Dictionary> ("LaunchAction");
    auto PA  = S->value <plist::Dictionary> ("ProfileAction");
    auto AnA = S->value <plist::Dictionary> ("AnalyzeAction");
    auto ArA = S->value <plist::Dictionary> ("ArchiveAction");

    if (LUV != nullptr) {
        _lastUpgradeVersion = LUV->value();
    }

    if (V != nullptr) {
        _version = V->value();
    }

    if (auto BA = S->value <plist::Dictionary> ("BuildAction")) {
        _buildAction = std::make_shared <BuildAction> ();
        if (!_buildAction->parse(BA))
            return false;
    }

    if (auto TA = S->value <plist::Dictionary> ("TestAction")) {
        _testAction = std::make_shared <TestAction> ();
        if (!_testAction->parse(TA))
            return false;
    }

    if (auto LA = S->value <plist::Dictionary> ("LaunchAction")) {
        _launchAction = std::make_shared <LaunchAction> ();
        if (!_launchAction->parse(LA))
            return false;
    }

    if (auto PA = S->value <plist::Dictionary> ("ProfileAction")) {
        _profileAction = std::make_shared <ProfileAction> ();
        if (!_profileAction->parse(PA))
            return false;
    }

    if (auto AA = S->value <plist::Dictionary> ("AnalyzeAction")) {
        _analyzeAction = std::make_shared <AnalyzeAction> ();
        if (!_analyzeAction->parse(AA))
            return false;
    }

    if (auto AA = S->value <plist::Dictionary> ("ArchiveAction")) {
        _archiveAction = std::make_shared <ArchiveAction> ();
        if (!_archiveAction->parse(AA))
            return false;
    }

    return true;
}

Scheme::shared_ptr Scheme::
Open(std::string const &name, std::string const &owner, std::string const &path)
{
    if (path.empty()) {
        errno = EINVAL;
        return nullptr;
    }

    if (!FSUtil::TestForRead(path.c_str()))
        return nullptr;

    std::string realPath = FSUtil::ResolvePath(path);
    if (realPath.empty())
        return nullptr;

    //
    // Parse simple XML
    //
    plist::Dictionary *plist = plist::Dictionary::ParseSimpleXML(path);
    if (plist == nullptr)
        return nullptr;

#if 0
    plist->dump(stdout);
#endif

    //
    // Parse the scheme dictionary and create the scheme object.
    //
    auto scheme = std::make_shared <Scheme> (name, owner);
    if (scheme->parse(plist)) {
        scheme->_path = realPath;
    } else {
        scheme = nullptr;
    }
                                                        
    //
    // Release the property list.
    //
    plist->release();

    return scheme;
}