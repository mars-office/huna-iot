class FileManager {
public:
    FileManager();
    ~FileManager();

    const char* readFile(const char* fileName);
    void deleteFileIfExists(const char* fileName);

private:
    const char* pem;
};