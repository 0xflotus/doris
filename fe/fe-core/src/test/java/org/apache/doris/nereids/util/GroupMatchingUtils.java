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

package org.apache.doris.nereids.util;

import org.apache.doris.nereids.memo.Group;
import org.apache.doris.nereids.memo.GroupExpression;
import org.apache.doris.nereids.pattern.GroupExpressionMatching;
import org.apache.doris.nereids.pattern.Pattern;
import org.apache.doris.nereids.trees.plans.Plan;

public class GroupMatchingUtils {

    public static boolean topDownFindMatching(Group group, Pattern<? extends Plan> pattern) {
        for (GroupExpression logicalExpr : group.getLogicalExpressions()) {
            if (topDownFindMatch(logicalExpr, pattern)) {
                return true;
            }
        }

        for (GroupExpression physicalExpr : group.getPhysicalExpressions()) {
            if (topDownFindMatch(physicalExpr, pattern)) {
                return true;
            }
        }
        return false;
    }

    public static boolean topDownFindMatch(GroupExpression groupExpression, Pattern<? extends Plan> pattern) {
        GroupExpressionMatching matchingResult = new GroupExpressionMatching(pattern, groupExpression);
        if (matchingResult.iterator().hasNext()) {
            return true;
        } else {
            for (Group childGroup : groupExpression.children()) {
                boolean checkResult = topDownFindMatching(childGroup, pattern);
                if (checkResult) {
                    return true;
                }
            }
        }
        return false;
    }
}
