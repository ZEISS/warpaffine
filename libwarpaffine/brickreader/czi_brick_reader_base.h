// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#pragma once

#include "../czi_helpers.h"
#include "../inc_libCZI.h"
#include "IBrickReader.h"
#include "../appcontext.h"
#include <map>
#include <memory>
#include <tuple>

class CziBrickReaderBase
{
private:
    AppContext& context_;
    libCZI::SubBlockStatistics statistics_;
    std::map<int, libCZI::PixelType> map_channelno_to_pixeltype_;
    std::shared_ptr<libCZI::ICZIReader> underlying_reader_;
protected:
    /// This structure gathers the metadata of a brick. The metadata is provided by the reader, and
    /// attached to the output of the brick reader.
    struct BrickMetadata
    {
        double stage_position_x;
        double stage_position_y;
        libCZI::XmlDateTime acquisition_datetime;

        void Clear();
    };
public:
    CziBrickReaderBase() = delete;

    CziBrickReaderBase(AppContext& context, const std::shared_ptr<libCZI::ICZIReader>& reader)
        : context_(context)
    {
        this->statistics_ = reader->GetStatistics();
        this->map_channelno_to_pixeltype_ = CziHelpers::GetMapOfChannelsToPixeltype(reader.get());
        this->underlying_reader_ = reader;
    }

    std::shared_ptr<libCZI::ICZIReader>& GetUnderlyingReaderBase()
    {
        return this->underlying_reader_;
    }

    AppContext& GetContextBase()
    {
        return this->context_;
    }

    const libCZI::SubBlockStatistics& GetStatistics()
    {
        return this->statistics_;
    }

    libCZI::PixelType GetPixelTypeForChannelNo(int c)
    {
        return this->map_channelno_to_pixeltype_[c];
    }

    /// Retrieves stage-position and acquisition-time metadata from a sub block.
    /// Stage-position and acquisition-time metadata are read only when the corresponding
    /// options are enabled in the application context. Otherwise, the returned metadata
    /// contains invalid/default values.
    ///
    /// \param  sub_block   The sub block from which to retrieve the metadata.
    ///
    /// \returns    The brick metadata retrieved from the sub block.
    BrickMetadata RetrieveBrickMetadataFromSubBlock(const libCZI::ISubBlock* sub_block);

    /// Fills out the metadata fields in a BrickCoordinateInfo structure.
    /// This copies the stage position and acquisition time from the supplied brick metadata.
    ///
    /// \param          brick_metadata          The brick metadata to copy.
    /// \param [out]    brick_coordinate_info   The information structure to fill out.
    static void FillOutInformationFromBrickMetadata(const BrickMetadata& brick_metadata, BrickCoordinateInfo* brick_coordinate_info);
};
