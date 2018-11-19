#ifndef SCENE_HPP
#define SCENE_HPP

#include "nodes.h"
#include <queue>
#include <pose_cov_ops/pose_cov_ops.h>

namespace uwds {

  /** @brief
   * This class represent the scene.
   */
  class Scene {
    public:
      /** @brief
       * Default constructor.
       */
      Scene() {
        root_id_ = NEW_UUID;
        nodes_ = boost::make_shared<Nodes>();
        uwds_msgs::Node root_node;
        root_node.id = root_id_;
        root_node.name = "root";
        root_node.position.pose.orientation.w = 1.0;
        nodes_->update(root_node);
      }

      /** @brief
       * Copy destructor.
       */
      Scene(const Scene& scene) = default;

      /** @brief
       * Move constructor.
       */
      Scene(Scene&& scene) = delete;

      /** @brief
       * Default destructor.
       */
      ~Scene() = default;

      /** @brief
       * This method update a node (or create one if new)
       *
       * @param node The node to update
       */
      void update(const NodePtr node)
      {
        nodes_->update(node);
      }

      /** @brief
       * This method update a node (or create one if new)
       *
       * @param node The node to update
       */
      void update(const Node node)
      {
        nodes_->update(node);
      }

      /** @brief
       * This method update a set of nodes (or create them if new)
       *
       * @param nodes The nodes to update
       */
      void update(const std::vector<Node> nodes)
      {
        nodes_->update(nodes);
      }

      /** @brief
       * This method update a set of nodes (or create them if new)
       *
       * @param nodes The nodes to update
       */
      void update(const std::vector<NodePtr> nodes)
      {
        nodes_->update(nodes);
      }

      /** @brief
       * This method remove a node
       *
       * @param id The node id to delete
       */
      void remove(const std::string id)
      {
        nodes_->remove(id);
      }

      /** @brief
       * This method remove a set of nodes
       *
       * @param ids The node ids to delete
       */
      void remove(const std::vector<std::string> ids)
      {
        nodes_->remove(ids);
      }

      /** @brief
       * This method set the root node ID
       *
       * @param root_id The root node ID
       */
      std::string setRootID(const std::string root_id) {root_id_=root_id;}

      /** @brief
       * This method return the root node
       *
       * @return The root node ID
       */
      std::string rootID() const {return root_id_;}

      /** @brief
       * This method return the nodes container.
       *
       * @return The nodes container
       */
      Nodes& nodes() {return * nodes_;}

      void reset(const std::string& new_root_id)
      {
        nodes_->reset();
        uwds_msgs::Node root_node;
        root_node.id = new_root_id;
        root_node.name = "root";
        root_node.position.pose.orientation.w = 1.0;
        setRootID(new_root_id);
        nodes_->update(root_node);
      }

      void reset()
      {
        nodes_->reset();
        uwds_msgs::Node root_node;
        root_node.id = root_id_;
        root_node.name = "root";
        root_node.position.pose.orientation.w = 1.0;
        nodes_->update(root_node);
      }

      /** @brief
       * Lock the scene.
       */
      void lock() {nodes_->lock();}

      /** @brief
       * Unlock the scene.
       */
      void unlock() {nodes_->unlock();}

      /** @brief
       * Get the given node parents.
       *
       * @param node_id The node ID
       * @return The parents of the node
       */
      std::vector<Node> getParents(const std::string& node_id)
      {
        Node current_node;
        std::vector<Node> parents;
        std::queue<Node> fifo;
        if (nodes()[node_id].parent != "")
        {
          fifo.push(nodes()[nodes()[node_id].parent]);
          do {
            current_node = fifo.front();
            fifo.pop();
            parents.push_back(current_node);
            if(current_node.parent!="")
              fifo.push(nodes()[current_node.parent]);
          } while(!fifo.empty());
        }
        return parents;
      }

      geometry_msgs::Pose getWorldPose(const std::string& node_id)
      {
        Node current_node;
        geometry_msgs::Pose final_pose;
        std::queue<Node> fifo;
        final_pose = nodes()[node_id].position.pose;
        if (nodes()[node_id].parent != "")
        {
          fifo.push(nodes()[nodes()[node_id].parent]);
          do {
            current_node = fifo.front();
            fifo.pop();
            pose_cov_ops::compose(current_node.position.pose, final_pose, final_pose);
            if(current_node.parent!="")
              fifo.push(nodes()[current_node.parent]);
          } while(!fifo.empty());
        }
        return final_pose;
      }

      geometry_msgs::PoseWithCovariance getWorldPoseWithCovariance(const std::string& node_id)
      {
        Node current_node;
        geometry_msgs::PoseWithCovariance final_pose;
        std::queue<Node> fifo;
        final_pose = nodes()[node_id].position;
        if (nodes()[node_id].parent != "")
        {
          fifo.push(nodes()[nodes()[node_id].parent]);
          do {
            current_node = fifo.front();
            fifo.pop();
            pose_cov_ops::compose(current_node.position, final_pose, final_pose);
            if(current_node.parent!="")
              fifo.push(nodes()[current_node.parent]);
          } while(!fifo.empty());
        }
        return final_pose;
      }

      bool has(const std::string id)
      {
        return nodes().has(id);
      }

      size_t size()
      {
        return nodes().size();
      }

    private:
      std::string root_id_;
      NodesPtr nodes_;
  };

  typedef boost::shared_ptr<uwds::Scene> ScenePtr;
  typedef boost::shared_ptr<uwds::Scene const> SceneConstPtr;
}

#endif
