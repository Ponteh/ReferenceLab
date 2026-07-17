#pragma once
#include <optional>
namespace referencelab {
enum class TransportSyncMode { timeline, restartOnPlay };
struct TransportCommand { bool available=false,play=false,pause=false,seek=false;double positionSeconds=0; };
class TransportController { public:TransportCommand update(std::optional<double> hostSeconds,bool hostPlaying,double referenceDuration,double offsetSeconds,TransportSyncMode);void reset()noexcept{wasPlaying=false;lastTarget=-1;}private:bool wasPlaying=false;double lastTarget=-1;};
}
