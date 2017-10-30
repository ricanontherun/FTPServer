#ifndef FTP_MD5_CHECKSUM_HPP
#define FTP_MD5_CHECKSUM_HPP

#include <openssl/md5.h>

class MD5_Checksum {
public:
    MD5_Checksum() {
        MD5_Init(&(this->context));
    }

    void Add(const std::string & data) {
        return this->Add(data.c_str(), data.length());
    }

    void Add(const char * data, size_t len) {
        MD5_Update(&(this->context), data, len);
    }

    const std::string & Finish() {
        MD5_Final(this->digest, &(this->context));

        char md5[33];
        for (int i = 0; i < 16; ++i) {
            sprintf(&md5[i*2], "%02x", (unsigned int)this->digest[i]);
        }

        this->md5 = std::string(md5);

        return this->md5;
    }

private:
    unsigned char digest[MD5_DIGEST_LENGTH];
    MD5_CTX context;
    std::string md5;
};

#endif //FTP_MD5_CHECKSUM_HPP
