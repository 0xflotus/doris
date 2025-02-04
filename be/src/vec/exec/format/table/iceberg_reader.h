// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements.  See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership.  The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License.  You may obtain a copy of the License at
//
//   http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied.  See the License for the
// specific language governing permissions and limitations
// under the License.

#pragma once

#include <vec/exec/format/parquet/parquet_common.h>

#include <queue>

#include "table_format_reader.h"
#include "vec/exec/format/generic_reader.h"
#include "vec/exprs/vexpr.h"

namespace doris::vectorized {

class IcebergTableReader : public TableFormatReader {
public:
    IcebergTableReader(GenericReader* file_format_reader, RuntimeProfile* profile,
                       RuntimeState* state, const TFileScanRangeParams& params);
    ~IcebergTableReader() override;
    Status init_row_filters(const TFileRangeDesc& range);
    void filter_rows(const TFileRangeDesc& range) override;

    Status get_next_block(Block* block, size_t* read_rows, bool* eof) override;

    Status set_fill_columns(
            const std::unordered_map<std::string, std::tuple<std::string, const SlotDescriptor*>>&
                    partition_columns,
            const std::unordered_map<std::string, VExprContext*>& missing_columns) override;

    Status get_columns(std::unordered_map<std::string, TypeDescriptor>* name_to_type,
                       std::unordered_set<std::string>* missing_cols) override;

public:
    enum { DATA, POSITON_DELELE, EQULITY_DELELE };
    struct PositionDeleteParams {
        int64_t low_bound_index = -1;
        int64_t upper_bound_index = -1;
        int64_t last_delete_row_index = -1;
        int64_t total_file_rows = 0;
    };

private:
    struct IcebergProfile {
        RuntimeProfile::Counter* _delete_files_init_time;
        RuntimeProfile::Counter* _delete_files_read_total_time;
    };
    RuntimeProfile* _profile;
    RuntimeState* _state;
    const TFileScanRangeParams& _params;
    std::vector<FieldSchema> _column_schemas;
    std::deque<std::unique_ptr<GenericReader>> _delete_file_readers;
    std::unique_ptr<GenericReader> _cur_delete_file_reader;
    PositionDeleteParams _position_delete_params;
    const FieldDescriptor* _delete_file_schema = nullptr;
    VExprContext* _data_path_conjunct_ctx = nullptr;
    IcebergProfile _iceberg_profile;
};

} // namespace doris::vectorized
