#include "TransportController.h"
#include <algorithm>
#include <cmath>
namespace referencelab {
TransportCommand TransportController::update(std::optional<double>host,bool playing,double duration,double offset,TransportSyncMode mode){TransportCommand c;if(!host){wasPlaying=playing;lastTarget=-1;return c;}c.available=true;c.play=playing;c.pause=!playing;if(duration<=0){wasPlaying=playing;return c;}if(mode==TransportSyncMode::restartOnPlay){if(playing&&!wasPlaying){c.seek=true;c.positionSeconds=std::clamp(offset,0.0,duration);}lastTarget=-1;}else{auto target=std::fmod(*host+offset,duration);if(target<0)target+=duration;auto discontinuity=lastTarget<0||std::abs(target-lastTarget)>.15;if(playing&&discontinuity){c.seek=true;c.positionSeconds=target;}lastTarget=target;}wasPlaying=playing;return c;}
}
