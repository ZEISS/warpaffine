// SPDX-FileCopyrightText: 2023 Carl Zeiss Microscopy GmbH
//
// SPDX-License-Identifier: MIT

#include "czi_brick_reader_base.h"
#include <limits>

using namespace std;

void CziBrickReaderBase::BrickMetadata::Clear()
{
    this->stage_position_x = std::numeric_limits<double>::quiet_NaN();
    this->stage_position_y = std::numeric_limits<double>::quiet_NaN();
    this->acquisition_datetime.SetToInvalid();
}

CziBrickReaderBase::BrickMetadata CziBrickReaderBase::RetrieveBrickMetadataFromSubBlock(const libCZI::ISubBlock* sub_block)
{
    BrickMetadata metadata;
    metadata.Clear();

    if (this->GetContextBase().GetCommandLineOptions().GetWriteStagePositionsInSubblockMetadata())
    {
        auto stage_position = CziHelpers::GetStagePositionFromXmlMetadata(sub_block);
        metadata.stage_position_x = std::get<0>(stage_position);
        metadata.stage_position_y = std::get<1>(stage_position);
    }

    if (this->GetContextBase().GetCommandLineOptions().GetWriteAcquisitionTimeInSubblockMetadata())
    {
        metadata.acquisition_datetime = CziHelpers::GetAcquisitionTimeFromXmlMetadata(sub_block);
    }

    return metadata;
}

void CziBrickReaderBase::FillOutInformationFromBrickMetadata(const BrickMetadata& brick_metadata, BrickCoordinateInfo* brick_coordinate_info)
{
    brick_coordinate_info->stage_x_position = brick_metadata.stage_position_x;
    brick_coordinate_info->stage_y_position = brick_metadata.stage_position_y;
    brick_coordinate_info->acquisition_time = brick_metadata.acquisition_datetime;
}
