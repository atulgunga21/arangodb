////////////////////////////////////////////////////////////////////////////////
/// DISCLAIMER
///
/// Copyright 2014-2016 ArangoDB GmbH, Cologne, Germany
/// Copyright 2004-2014 triAGENS GmbH, Cologne, Germany
///
/// Licensed under the Apache License, Version 2.0 (the "License");
/// you may not use this file except in compliance with the License.
/// You may obtain a copy of the License at
///
///     http://www.apache.org/licenses/LICENSE-2.0
///
/// Unless required by applicable law or agreed to in writing, software
/// distributed under the License is distributed on an "AS IS" BASIS,
/// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
/// See the License for the specific language governing permissions and
/// limitations under the License.
///
/// Copyright holder is ArangoDB GmbH, Cologne, Germany
///
/// @author Jan Steemann
////////////////////////////////////////////////////////////////////////////////

#include "QueryProfile.h"

#include "Aql/Query.h"
#include "Aql/QueryList.h"
#include "VocBase/vocbase.h"

#include <velocypack/Builder.h>
#include <velocypack/velocypack-aliases.h>

using namespace arangodb;
using namespace arangodb::aql;

/// @brief create a profile
QueryProfile::QueryProfile(Query* query)
    : query(query), results(), stamp(query->startTime()), tracked(false) {
  auto queryList = query->vocbase()->queryList();

  try {
    tracked = queryList->insert(query);
  } catch (...) {
  }
}

/// @brief destroy a profile
QueryProfile::~QueryProfile() {
  // only remove from list when the query was inserted into it...
  if (tracked) {
    auto queryList = query->vocbase()->queryList();

    try {
      queryList->remove(query);
     } catch (...) {
    }
  }
}

/// @brief sets a state to done
void QueryProfile::setDone(QueryExecutionState::ValueType state) {
  double const now = TRI_microtime();

  if (state != QueryExecutionState::ValueType::INVALID_STATE) {
    // record duration of state
    results.emplace_back(state, now - stamp);
  }

  // set timestamp
  stamp = now;
}

/// @brief convert the profile to VelocyPack
std::shared_ptr<VPackBuilder> QueryProfile::toVelocyPack() {
  auto result = std::make_shared<VPackBuilder>();
  {
    VPackObjectBuilder b(result.get());
    for (auto const& it : results) {
      result->add(QueryExecutionState::toString(it.first),
                  VPackValue(it.second));
    }
  }
  return result;
}
