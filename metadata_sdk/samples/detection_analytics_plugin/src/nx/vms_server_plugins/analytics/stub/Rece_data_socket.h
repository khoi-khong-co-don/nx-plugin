#include <boost/archive/iterators/binary_from_base64.hpp>
#include <boost/archive/iterators/transform_width.hpp>

#include "face_detection_renovation/i_event_controller.h"
#include <vector>
#include <string>
#include "Logger.h"
#include "json11.hpp"
#include "i_camera.h"

#ifdef _WIN32
#define DWORD       unsigned long
#else
#define DWORD       unsigned int
#endif

namespace nx {
    namespace vms_server_plugins {
        namespace analytics {
            namespace stub {
         

                //  class WorkerThread;
                class DataSocket {

                public:

                public:
                    void dataSocket(int ai_module, std::string dataSocket, std::string User);

                    // base64 to byte and dword
                    std::vector<unsigned char> base64_decode_to_bytes(const std::string& base64_string);
                    DWORD get_buffer_size(const std::string& base64_string);
                    void capture(BYTE* pBuffer, DWORD dwBufSize, LDWORD User);
                private:

                };
            }
        }
    }
}