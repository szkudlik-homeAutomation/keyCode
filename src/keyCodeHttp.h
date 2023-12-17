#pragma once

#if CONFIG_HTTP_SERVER

#include "Common_code/Network/httpServer.h"
#include "http_binaries.h"

class tDefaultPageServlet :  public tHttpServlet
{
public:
   tDefaultPageServlet() : tHttpServlet() {}

  virtual bool ProcessAndResponse()
  {
    pOwner->SendFlashString(defaultPage_http_raw,defaultPage_http_raw_len);
    SendVersionAndPageClose();
    return false;
  }

  virtual ~tDefaultPageServlet() {};
};


#endif CONFIG_HTTP_SERVER
