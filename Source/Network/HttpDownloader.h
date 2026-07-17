#pragma once

#include <juce_core/juce_core.h>
#include <functional>

namespace referencelab {
enum class ReferenceError { none,notFound,unsupportedFormat,decodeFailure,networkUnavailable,
                            permissionDenied,invalidMetadata,cancelled,tooLarge,unknown };

struct HttpDownloadOptions {
    juce::int64 maximumBytes = 64ll * 1024ll * 1024ll;
    int timeoutMs = 20000, redirects = 5, maximumAttempts = 2;
    juce::String resourceName = "risorsa remota";
};

struct HttpDownloadResult {
    juce::MemoryBlock data;
    juce::StringPairArray headers;
    juce::String message;
    ReferenceError error = ReferenceError::none;
    int statusCode = 0;
    bool succeeded() const noexcept { return error == ReferenceError::none; }
};

class HttpDownloader {
public:
    using ProgressCallback = std::function<void(float)>;
    using CancellationCheck = std::function<bool()>;
    static HttpDownloadResult download(const juce::URL&,const HttpDownloadOptions&,
                                       ProgressCallback = {},CancellationCheck = {});
};
}
