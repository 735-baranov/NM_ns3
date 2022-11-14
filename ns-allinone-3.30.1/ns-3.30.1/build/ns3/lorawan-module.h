
#ifdef NS3_MODULE_COMPILATION
# error "Do not include ns3 module aggregator headers from other modules; these are meant only for end user scripts."
#endif

#ifndef NS3_MODULE_LORAWAN
    

// Module headers:
#include "lorawan-enddevice-application.h"
#include "lorawan-error-model.h"
#include "lorawan-frame-header.h"
#include "lorawan-gateway-application.h"
#include "lorawan-helper.h"
#include "lorawan-interference-helper.h"
#include "lorawan-lqi-tag.h"
#include "lorawan-mac-header.h"
#include "lorawan-mac.h"
#include "lorawan-net-device.h"
#include "lorawan-phy.h"
#include "lorawan-spectrum-signal-parameters.h"
#include "lorawan-spectrum-value-helper.h"
#include "lorawan.h"
#endif
