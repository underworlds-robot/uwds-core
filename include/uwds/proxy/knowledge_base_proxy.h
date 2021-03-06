#ifndef KNOWLEDGE_BASE_PROXY_HPP
#define KNOWLEDGE_BASE_PROXY_HPP

#include "proxy.h"

using namespace std;
using namespace uwds_msgs;

namespace uwds {

  class QueryKnowledgeBase : public ServiceProxy<QueryInContext, string>
  {
  public:
    QueryKnowledgeBase(NodeHandlePtr nh, NodeHandlePtr pnh, ClientPtr client, string world_name):ServiceProxy<QueryInContext, string>(nh, pnh, client, "uwds/query_knowledge_base")
    {
      world_name_ = world_name;
    }

  protected:
    QueryInContext fillRequest(string query)
    {
      QueryInContext query_srv;
      query_srv.request.ctxt.client = *client_;
      query_srv.request.ctxt.world = world_name_;
      query_srv.request.query = query;
      return query_srv;
    }

    string world_name_;
  };

  typedef boost::shared_ptr<QueryKnowledgeBase> QueryKnowledgeBasePtr;
  typedef boost::shared_ptr<QueryKnowledgeBase const> QueryKnowledgeBaseConstPtr;

  class KnowledgeBaseProxy
  {
  public:
    KnowledgeBaseProxy(NodeHandlePtr nh, NodeHandlePtr pnh, ClientPtr client, string world_name)
    {
      query_proxy_ = boost::make_shared<QueryKnowledgeBase>(nh, pnh, client, world_name);
    }

    vector<string> queryKnowledgeBase(string query)
    {
      QueryInContext query_srv = query_proxy_->call(query);
      if(!query_srv.response.success)
        ROS_ERROR("[%s::knowledge] Exception occured when processing '%s' query", query_proxy_->client().name.c_str(), query.c_str());
      return query_srv.response.result;
    }
  protected:
    QueryKnowledgeBasePtr query_proxy_;
  };

  typedef boost::shared_ptr<KnowledgeBaseProxy> KnowledgeBaseProxyPtr;
  typedef boost::shared_ptr<KnowledgeBaseProxy const> KnowledgeBaseProxyConstPtr;

}

#endif
