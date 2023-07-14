/* Copyright (C) 2020 G'k
 * This Source Code Form is subject to the terms of the Mozilla Public
 * License, v. 2.0. If a copy of the MPL was not distributed with this
 * file, You can obtain one at https://mozilla.org/MPL/2.0/. */

#include "Commands/Textures/TextureConvert.hpp"

#include "Commands/CommandBook.hpp"
#include "utils.hpp"

using namespace cao;

SCENARIO("Converting a texture")
{
    TextureConvert convert;
    PerFileSettings sets;
    sets.slTextureUnwantedFormats = {DXGI_FORMAT_B5G6R5_UNORM};

    GIVEN("An unmodified image using an incompatible format, and settings with "
          "necessary opt enabled")
    {
        sets.bTexturesNecessary = true;
        auto file               = getStandardTextureFile(sets, false, DXGI_FORMAT_B5G6R5_UNORM);

        WHEN("The file is scanned")
        {
            CHECK_EQ(convert.isApplicable(*file), CommandState::Ready);
        }
    }
    GIVEN("An unmodified image using a compatible format, and settings with necessary opt enabled")
    {
        sets.bTexturesNecessary = true;
        auto file               = getStandardTextureFile(sets, false);

        WHEN("The file is scanned")
        {
            CHECK_EQ(convert.isApplicable(*file), CommandState::NotRequired);
        }
    }
    GIVEN("An unmodified image using a incompatible format, and settings with necessary opt disabled")
    {
        sets.bTexturesNecessary = false;
        auto file               = getStandardTextureFile(sets, false, DXGI_FORMAT_B5G6R5_UNORM);

        WHEN("The file is scanned")
        {
            CHECK_EQ(convert.isApplicable(*file), CommandState::NotRequired);
        }
    }
    GIVEN("A modified image using a compatible format, and settings with necessary opt enabled")
    {
        sets.bTexturesNecessary = true;
        auto file               = getStandardTextureFile(sets, true);

        WHEN("The file is scanned")
        {
            CHECK_EQ(convert.isApplicable(*file), CommandState::Ready);
        }
    }
    GIVEN("A modified image using the same format as the output format, and settings with necessary opt "
          "enabled")
    {
        sets.eTexturesFormat    = DXGI_FORMAT_A8_UNORM;
        sets.bTexturesNecessary = true;
        auto file               = getStandardTextureFile(sets, true, DXGI_FORMAT_A8_UNORM);

        WHEN("The file is scanned")
        {
            CHECK_EQ(convert.isApplicable(*file), CommandState::NotRequired);
        }
    }
}
