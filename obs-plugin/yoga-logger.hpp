int yoga_logger(
    YGConfigRef config,
    YGNodeRef node,
    YGLogLevel level,
    const char* format,
    va_list args
) {
    UNUSED_PARAMETER(config);
    UNUSED_PARAMETER(node);

    int blog_level = LOG_ERROR;

    switch (level) {
        case YGLogLevelVerbose:
        case YGLogLevelDebug:
            blog_level = LOG_DEBUG;
            break;

        case YGLogLevelInfo:
            blog_level = LOG_INFO;
            break;

        case YGLogLevelWarn:
            blog_level = LOG_WARNING;
            break;

        case YGLogLevelError:
        case YGLogLevelFatal:
            blog_level = LOG_ERROR;
            break;
    }

    auto new_format = std::string("[react-obs] [yoga] ") + format;

    blogva(blog_level, new_format.c_str(), args);

    return 0;
}
