/*
 * Licensed to the Apache Software Foundation (ASF) under one or more
 * contributor license agreements.  See the NOTICE file distributed with
 * this work for additional information regarding copyright ownership.
 * The ASF licenses this file to You under the Apache License, Version 2.0
 * (the "License"); you may not use this file except in compliance with
 * the License.  You may obtain a copy of the License at
 *
 *    http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <DataTypes/DataTypesNumber.h>
#include <Parser/FunctionParser.h>

namespace local_engine
{
class SparkFunctionSparkPartitionIDParser : public FunctionParser
{
public:
    explicit SparkFunctionSparkPartitionIDParser(ParserContextPtr parser_context_) : FunctionParser(parser_context_) { }
    ~SparkFunctionSparkPartitionIDParser() override = default;
    static constexpr auto name = "spark_partition_id";
    String getName() const override { return name; }

    const DB::ActionsDAG::Node *
    parse(const substrait::Expression_ScalarFunction & substrait_func, DB::ActionsDAG & actions_dag) const override
    {
        /// Spark's SparkPartitionID returns IntegerType; the partition index is fixed per task, so emit it as a constant.
        const DB::DataTypePtr type = std::make_shared<DB::DataTypeInt32>();
        Int32 partition_index = static_cast<Int32>(parser_context->getPartitionIndex());

        const auto * const_node = &actions_dag.addColumn(
            DB::ColumnWithTypeAndName(type->createColumnConst(1, partition_index), type, getUniqueName("spark_partition_id")));
        return convertNodeTypeIfNeeded(substrait_func, const_node, actions_dag);
    }
};
static FunctionParserRegister<SparkFunctionSparkPartitionIDParser> register_spark_partition_id;
}
