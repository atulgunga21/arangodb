/*global applicationContext*/

////////////////////////////////////////////////////////////////////////////////
/// @brief A Foxx.Controller to show all Foxx Applications
///
/// @file
///
/// DISCLAIMER
///
/// Copyright 2010-2013 triagens GmbH, Cologne, Germany
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
/// Copyright holder is triAGENS GmbH, Cologne, Germany
///
/// @author Michael Hackstein
/// @author Copyright 2011-2013, triAGENS GmbH, Cologne, Germany
////////////////////////////////////////////////////////////////////////////////

(function() {
  "use strict";

  // Initialize a new FoxxController called controller under the urlPrefix: "cluster".
  var FoxxController = require("@arangodb/foxx").Controller,
    UnauthorizedError = require("http-errors").Unauthorized,
    internal = require("internal"),
    controller = new FoxxController(applicationContext),
    cluster = require("@arangodb/cluster"),
    load = require("internal").download,
    db = require("internal").db,
    _ = require("lodash");

  controller.activateSessions({
    autoCreateSession: false,
    cookie: {name: "arango_sid_" + db._name()}
  });

  controller.allRoutes
  .errorResponse(UnauthorizedError, 401, "unauthorized")
  .onlyIf(function (req, res) {
    if (!internal.options()["server.disable-authentication"] && (!req.session || !req.session.get('uid'))) {
      throw new UnauthorizedError();
    }
  });

  /** Plan and start a new cluster
   *
   * This will plan a new cluster with the information
   * given in the body
   */
  controller.get("/amICoordinator", function(req, res) {
    res.json(cluster.isCoordinator());
  });

  if (cluster.isCluster()) {
    // only make these functions available in cluster mode!
    var Communication = require("@arangodb/cluster/agency-communication"),
    comm = new Communication.Communication(),
    beats = comm.sync.Heartbeats(),
    diff = comm.diff.current,
    servers = comm.current.DBServers(),
    dbs = comm.current.Databases(),
    coords = comm.current.Coordinators();


    /** Get the type of the cluster
     *
     * Returns a string containing the cluster type
     * Possible anwers:
     * - testSetup
     * - symmetricalSetup
     * - asymmetricalSetup
     *
     */
    controller.get("/ClusterType", function(req, res) {
      // Not yet implemented
      res.json({
        type: "symmetricSetup"
      });
    });

    /** Get all DBServers
    *
    * Get a list of all running and expected DBServers
    * within the cluster
    */
    controller.get("/DBServers", function(req, res) {
      var resList = [],
        list = servers.getList(),
        diffList = diff.DBServers(),
        didBeat = beats.didBeat(),
        serving = beats.getServing();

      _.each(list, function(v, k) {
        v.name = k;
        resList.push(v);
        if (!_.contains(didBeat, k)) {
          v.status = "critical";
          return;
        }
        if (v.role === "primary" && !_.contains(serving, k)) {
          v.status = "warning";
          return;
        }
        v.status = "ok";
      });
      _.each(diffList.missing, function(v) {
        v.status = "missing";
        resList.push(v);
      });
      res.json(resList);
    });

    controller.get("/Coordinators", function(req, res) {
      var resList = [],
        list = coords.getList(),
        diffList = diff.Coordinators(),
        didBeat = beats.didBeat();

      _.each(list, function(v, k) {
        v.name = k;
        resList.push(v);
        if (!_.contains(didBeat, k)) {
          v.status = "critical";
          return;
        }
        v.status = "ok";
      });
      _.each(diffList.missing, function(v) {
        v.status = "missing";
        resList.push(v);
      });
      res.json(resList);
    });

    controller.get("/Databases", function(req, res) {
      var list = dbs.getList();
      res.json(_.map(list, function(d) {
        return {name: d};
      }));
    });

    controller.get("/:dbname/Collections", function(req, res) {
      var dbname = req.params("dbname"),
        selected = dbs.select(dbname);
      try {
        res.json(_.map(selected.getCollections(),
          function(c) {
            return {name: c};
          })
        );
      } catch(e) {
        res.json([]);
      }
    });

    controller.get("/:dbname/:colname/Shards", function(req, res) {
      var dbname = req.params("dbname"),
        colname = req.params("colname"),
        selected = dbs.select(dbname).collection(colname);
      res.json(selected.getShardsByServers());
    });

    controller.get("/:dbname/:colname/Shards/:servername", function(req, res) {
      var dbname = req.params("dbname"),
        colname = req.params("colname"),
        servername = req.params("servername"),
        selected = dbs.select(dbname).collection(colname);
      res.json(_.map(selected.getShardsForServer(servername),
        function(c) {
          return {id: c};
        })
      );
    });

  } // end isCluster()

}());
