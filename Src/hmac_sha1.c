#include "hmac_sha1.h"

int32_t Mbedtls_SHA1_HMAC_Compute(uint8_t* InputMessage,
                                uint32_t InputMessageLength,
                                uint8_t *HMAC_key,
                                uint32_t HMAC_keyLength,
                                uint8_t *MessageDigest,
                                int32_t* MessageDigestLength);



int get_mqtt_password(const char* content, const char* devicesecret, char* password)
{
  //mqttPassword: sign_hmac(deviceSecret,content)

  char digest[20];
  char sig_hmac[44]= {'\0'};

  int32_t password_len;
  uint32_t i=0;
  
  memset(digest,0,sizeof(digest));

  //HMAC SHA1
  //transform to HEX string
  Mbedtls_SHA1_HMAC_Compute((uint8_t*)content,strlen(content),(uint8_t*)devicesecret,strlen(devicesecret),(uint8_t*)digest,&password_len);
  for(i=0;i<20;i++)
    sprintf(&sig_hmac[i*2],"%02x",(unsigned int)digest[i]);

  memcpy(password,sig_hmac,44);
  //printf("MQTT password generated successfully:%s\n",&password[0]);
  return 0;
}
  


int32_t Mbedtls_SHA1_HMAC_Compute(uint8_t* InputMessage,
                                uint32_t InputMessageLength,
                                uint8_t *HMAC_key,
                                uint32_t HMAC_keyLength,
                                uint8_t *MessageDigest,
                                int32_t* MessageDigestLength)
{
  mbedtls_md_context_t sha1_ctx;
  uint32_t ret = 0;

  mbedtls_md_init(&sha1_ctx);

  ret = mbedtls_md_setup(&sha1_ctx,mbedtls_md_info_from_type(MBEDTLS_MD_SHA1),1);

  /* check for initialization errors */
  if (ret == 0)
  {
    /* Add data to be hashed */
    if((ret=mbedtls_md_hmac_starts(&sha1_ctx,HMAC_key,HMAC_keyLength))!=0)
      goto exit;
    if((ret=mbedtls_md_hmac_update(&sha1_ctx,InputMessage,InputMessageLength))!=0)
      goto exit;
    if((ret=mbedtls_md_hmac_finish(&sha1_ctx,MessageDigest))!=0)
      goto exit;
    *MessageDigestLength = strlen((const char *)MessageDigest);
  }

exit:
  mbedtls_md_free(&sha1_ctx);
  return ret;
}
