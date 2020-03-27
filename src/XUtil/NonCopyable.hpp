#ifndef _NONCOPYABLE_H_
#define _NONCOPYABLE_H_

namespace VOICEENGINEAPI {
    class NonCopyable {
    public:
        NonCopyable(const NonCopyable &) = delete; // deleted
        NonCopyable &operator=(const NonCopyable &) = delete; // deleted
        NonCopyable() = default;   // available
    };
}
#endif
