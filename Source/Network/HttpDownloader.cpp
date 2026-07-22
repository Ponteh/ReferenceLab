#include "HttpDownloader.h"
#include <array>

namespace referencelab {
namespace {
bool retryable(int status) noexcept { return status==0||status==408||status==429||status>=500; }
HttpDownloadResult failure(ReferenceError error,const juce::String&message,int status=0){HttpDownloadResult result;result.error=error;result.message=message;result.statusCode=status;return result;}
}

HttpDownloadResult HttpDownloader::download(const juce::URL&url,const HttpDownloadOptions&settings,
                                             ProgressCallback progress,CancellationCheck cancelled) {
    const auto attempts=juce::jmax(1,settings.maximumAttempts);
    for(int attempt=0;attempt<attempts;++attempt){
        if(cancelled&&cancelled())return failure(ReferenceError::cancelled,"Operation cancelled");
        if(progress)progress(0.f);
        int status=0;juce::StringPairArray headers;
        auto options=juce::URL::InputStreamOptions(juce::URL::ParameterHandling::inAddress)
            .withConnectionTimeoutMs(settings.timeoutMs).withNumRedirectsToFollow(settings.redirects)
            .withStatusCode(&status).withResponseHeaders(&headers);
        auto input=url.createInputStream(options);
        if(!input){if(attempt+1<attempts){juce::Thread::sleep(150*(attempt+1));continue;}return failure(ReferenceError::networkUnavailable,"Unable to connect to "+settings.resourceName,status);}
        if(status<200||status>=300){
            if(retryable(status)&&attempt+1<attempts){juce::Thread::sleep(150*(attempt+1));continue;}
            auto cloudflare=headers.getValue("cf-mitigated",{}).equalsIgnoreCase("challenge");
            auto error=status==404?ReferenceError::notFound:status==401||status==403?ReferenceError::permissionDenied:ReferenceError::networkUnavailable;
            auto message=cloudflare?"Access blocked by Cloudflare verification for "+settings.resourceName+" (HTTP "+juce::String(status)+")":"Failed to download "+settings.resourceName+" (HTTP "+juce::String(status)+")";
            return failure(error,message,status);
        }
        auto expected=headers.getValue("Content-Length",{}).getLargeIntValue();
        if(expected>settings.maximumBytes)return failure(ReferenceError::tooLarge,settings.resourceName+" is too large",status);
        juce::MemoryOutputStream output;std::array<char,65536>buffer{};
        while(true){
            if(cancelled&&cancelled())return failure(ReferenceError::cancelled,"Operation cancelled",status);
            auto count=input->read(buffer.data(),(int)buffer.size());if(count<=0)break;
            if((juce::int64)output.getDataSize()+count>settings.maximumBytes)return failure(ReferenceError::tooLarge,settings.resourceName+" is too large",status);
            output.write(buffer.data(),(size_t)count);if(progress&&expected>0)progress(juce::jlimit(0.f,1.f,(float)((double)output.getDataSize()/expected)));
        }
        if(output.getDataSize()==0)return failure(ReferenceError::networkUnavailable,settings.resourceName+" is empty",status);
        if(expected>0&&(juce::int64)output.getDataSize()<expected){if(attempt+1<attempts){juce::Thread::sleep(150*(attempt+1));continue;}return failure(ReferenceError::networkUnavailable,"Incomplete download of "+settings.resourceName,status);}
        HttpDownloadResult result;result.statusCode=status;result.headers=headers;result.data.append(output.getData(),output.getDataSize());if(progress)progress(1.f);return result;
    }
    return failure(ReferenceError::unknown,"Download failed");
}
}
