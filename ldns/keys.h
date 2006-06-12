/*
 * 
 * keys.h
 *
 * priv key definitions
 *
 * a Net::DNS like library for C
 *
 * (c) NLnet Labs, 2005-2006
 *
 * See the file LICENSE for the license
 */

#ifndef LDNS_KEYS_H
#define LDNS_KEYS_H

#ifdef HAVE_SSL
#include <openssl/ssl.h>
#endif /* HAVE_SSL */
#include <ldns/dnssec.h>
#include <ldns/util.h>

extern ldns_lookup_table ldns_signing_algorithms[];

#define LDNS_KEY_ZONE_KEY 0x0100
#define LDNS_KEY_SEP_KEY 0x0001

/**
 * Algorithms used in dns
 */
enum ldns_enum_algorithm
{
        LDNS_RSAMD5             = 1,
        LDNS_DH                 = 2,
        LDNS_DSA                = 3,
        LDNS_ECC                = 4,
        LDNS_RSASHA1            = 5,
        LDNS_INDIRECT           = 252,
        LDNS_PRIVATEDNS         = 253,
        LDNS_PRIVATEOID         = 254
};
typedef enum ldns_enum_algorithm ldns_algorithm;

/**
 * Hashing algorithms use in the DS record
 */
enum ldns_enum_hash
{
        LDNS_SHA1               = 1,
        LDNS_SHA256             = 2
};
typedef enum ldns_enum_hash ldns_hash;

/**
 * Algorithms used in dns for signing
 */
enum ldns_enum_signing_algorithm
{
	LDNS_SIGN_RSAMD5	 = LDNS_RSAMD5,
	LDNS_SIGN_RSASHA1	 = LDNS_RSASHA1,
	LDNS_SIGN_DSA		 = LDNS_DSA,
	LDNS_SIGN_HMACMD5	 = 150	/* not official! */
};
typedef enum ldns_enum_signing_algorithm ldns_signing_algorithm;

/**
 * General key structure, can contain all types of keys that
 * are used in DNSSEC. Mostly used to store private keys, since
 * public keys can also be stored in a \ref ldns_rr with type
 * \ref LDNS_RR_TYPE_DNSKEY.
 *
 * This structure can also store some variables that influence the
 * signatures generated by signing with this key, for instance the
 * inception date.
 */
struct ldns_struct_key {
	ldns_signing_algorithm _alg;
	/** Storage pointers for the types of keys supported */
	union {
#ifdef HAVE_SSL
		RSA	*rsa;
		DSA	*dsa;
#endif /* HAVE_SSL */
		unsigned char *hmac;
	} _key;
	/** Depending on the key we can have extra data */
	union {
                /** Some values that influence generated signatures */
		struct {
			/** The TTL of the rrset that is currently signed */
			uint32_t orig_ttl;
			/** The inception date of signatures made with this key. */
			uint32_t inception;
			/** The expiration date of signatures made with this key. */
			uint32_t expiration;
			/** The keytag of this key. */
			uint16_t keytag;
			/** The dnssec key flags as specified in RFC4035, like ZSK and KSK */
			uint16_t flags;
		}  dnssec;
	} _extra;
	/** Owner name of the key */
	ldns_rdf *_pubkey_owner;
};
typedef struct ldns_struct_key ldns_key;

/**
 * Same as rr_list, but now for keys 
 */
struct ldns_struct_key_list
{
	size_t _key_count;
	ldns_key **_keys;
};
typedef struct ldns_struct_key_list ldns_key_list;


/**
 * Creates a new empty key list
 * \return a new ldns_key_list structure pointer
 */
ldns_key_list *ldns_key_list_new();

/** 
 * Creates a new empty key structure
 * \return a new ldns_key * structure
 */
ldns_key *ldns_key_new();

/**
 * Creates a new key based on the algorithm
 *
 * \param[in] a The algorithm to use
 * \param[in] size the number of bytes for the keysize
 * \return a new ldns_key structure with the key
 */
ldns_key *ldns_key_new_frm_algorithm(ldns_signing_algorithm a, uint16_t size);

/**
 * Creates a new priv key based on the 
 * contents of the file pointed by fp.
 *
 * The file should be in Private-key-format v1.2.
 *
 * \param[out] k the new ldns_key structure
 * \param[in] fp the file pointer to use
 * \return an error or LDNS_STATUS_OK
 */
ldns_status ldns_key_new_frm_fp(ldns_key **k, FILE *fp);

/**
 * Creates a new private key based on the 
 * contents of the file pointed by fp
 *
 * The file should be in Private-key-format v1.2.
 *
 * \param[out] k the new ldns_key structure
 * \param[in] fp the file pointer to use
 * \param[in] line_nr pointer to an integer containing the current line number (for debugging purposes)
 * \return an error or LDNS_STATUS_OK
 */
ldns_status ldns_key_new_frm_fp_l(ldns_key **k, FILE *fp, int *line_nr);

#ifdef HAVE_SSL
/**
 * frm_fp helper function. This function parsed the
 * remainder of the (RSA) priv. key file generated from bind9
 * \param[in] fp the file to parse
 * \return NULL on failure otherwise a RSA structure
 */
RSA *ldns_key_new_frm_fp_rsa(FILE *fp);
#endif /* HAVE_SSL */

#ifdef HAVE_SSL
/**
 * frm_fp helper function. This function parsed the
 * remainder of the (RSA) priv. key file generated from bind9
 * \param[in] fp the file to parse
 * \param[in] line_nr pointer to an integer containing the current line number (for debugging purposes)
 * \return NULL on failure otherwise a RSA structure
 */
RSA *ldns_key_new_frm_fp_rsa_l(FILE *fp, int *line_nr);
#endif /* HAVE_SSL */

#ifdef HAVE_SSL
/**
 * frm_fp helper function. This function parsed the
 * remainder of the (DSA) priv. key file generated from bind9
 * \param[in] fp the file to parse
 * \return NULL on failure otherwise a RSA structure
 */
DSA *ldns_key_new_frm_fp_dsa(FILE *fp);
#endif /* HAVE_SSL */

#ifdef HAVE_SSL
/**
 * frm_fp helper function. This function parsed the
 * remainder of the (DSA) priv. key file generated from bind9
 * \param[in] fp the file to parse
 * \param[in] line_nr pointer to an integer containing the current line number (for debugging purposes)
 * \return NULL on failure otherwise a RSA structure
 */
DSA *ldns_key_new_frm_fp_dsa_l(FILE *fp, int *line_nr);
#endif /* HAVE_SSL */

/* acces write functions */
/**
 * Set the key's algorithm
 * \param[in] k the key
 * \param[in] l the algorithm
 */
void ldns_key_set_algorithm(ldns_key *k, ldns_signing_algorithm l);
#ifdef HAVE_SSL
/**
 * Set the key's rsa data
 * \param[in] k the key
 * \param[in] r the rsa data
 */
void ldns_key_set_rsa_key(ldns_key *k, RSA *r);
/**
 * Set the key's dsa data
 * \param[in] k the key
 * \param[in] d the dsa data
 */
void ldns_key_set_dsa_key(ldns_key *k, DSA *d);
#endif /* HAVE_SSL */
/**
 * Set the key's hmac data
 * \param[in] k the key
 * \param[in] hmac the hmac data
 */
void ldns_key_set_hmac_key(ldns_key *k, unsigned char *hmac);
/**
 * Set the key's original ttl
 * \param[in] k the key
 * \param[in] t the ttl
 */
void ldns_key_set_origttl(ldns_key *k, uint32_t t);
/**
 * Set the key's inception date (seconds after epoch)
 * \param[in] k the key
 * \param[in] i the inception
 */
void ldns_key_set_inception(ldns_key *k, uint32_t i);
/**
 * Set the key's expiration date (seconds after epoch)
 * \param[in] k the key
 * \param[in] e the expiration
 */
void ldns_key_set_expiration(ldns_key *k, uint32_t e);
/**
 * Set the key's pubkey owner
 * \param[in] k the key
 * \param[in] r the owner
 */
void ldns_key_set_pubkey_owner(ldns_key *k, ldns_rdf *r);
/**
 * Set the key's key tag
 * \param[in] k the key
 * \param[in] tag the keytag
 */
void ldns_key_set_keytag(ldns_key *k, uint16_t tag);
/**
 * Set the key's flags
 * \param[in] k the key
 * \param[in] flags the flags
 */
void ldns_key_set_flags(ldns_key *k, uint16_t flags);
/**
 * Set the keylist's key count to count
 * \param[in] key the key
 * \param[in] count the cuont
 */
void ldns_key_list_set_key_count(ldns_key_list *key, size_t count);

/**     
 * pushes a key to a keylist
 * \param[in] key_list the key_list to push to 
 * \param[in] key the key to push 
 * \return false on error, otherwise true
 */      
bool ldns_key_list_push_key(ldns_key_list *key_list, ldns_key *key);

/**
 * returns the number of keys in the key list
 * \param[in] key_list the key_list
 * \return the numbers of keys in the list
 */
size_t ldns_key_list_key_count(const ldns_key_list *key_list);

/**
 * returns a pointer to the key in the list at the given position
 * \param[in] key the key
 * \param[in] nr the position in the list
 * \return the key
 */
ldns_key *ldns_key_list_key(const ldns_key_list *key, size_t nr);

#ifdef HAVE_SSL
/**
 * returns the (openssl) RSA struct contained in the key
 * \param[in] k the key to look in
 * \return the RSA * structure in the key
 */
RSA *ldns_key_rsa_key(const ldns_key *k);
#endif /* HAVE_SSL */

/**
 * returns the (openssl) DSA struct contained in the key
 */
#ifdef HAVE_SSL
DSA *ldns_key_dsa_key(const ldns_key *k);
#endif /* HAVE_SSL */

/**
 * return the signing alg of the key
 * \param[in] k the key
 * \return the algorithm
 */
ldns_signing_algorithm ldns_key_algorithm(const ldns_key *k);
/**
 * return the hmac key 
 * \param[in] k the key
 * \return the hmac key
 */
unsigned char *ldns_key_hmac_key(const ldns_key *k);
/**
 * return the original ttl of the key
 * \param[in] k the key
 * \return the original ttl
 */
uint32_t ldns_key_origttl(const ldns_key *k);
/**
 * return the key's inception date
 * \param[in] k the key
 * \return the inception date
 */
uint32_t ldns_key_inception(const ldns_key *k);
/**
 * return the key's expiration date
 * \param[in] k the key
 * \return the experiration date
 */
uint32_t ldns_key_expiration(const ldns_key *k);
/**
 * return the keytag
 * \param[in] k the key
 * \return the keytag
 */
uint16_t ldns_key_keytag(const ldns_key *k);
/**
 * return the public key's owner
 * \param[in] k the key
 * \return the owner
 */
ldns_rdf *ldns_key_pubkey_owner(const ldns_key *k);
/**
 * return the flag of the key
 * \param[in] k the key
 * \return the flag
 */
uint16_t ldns_key_flags(const ldns_key *k);

/**     
 * pops the last rr from a keylist
 * \param[in] key_list the rr_list to pop from
 * \return NULL if nothing to pop. Otherwise the popped RR
 */
ldns_key *ldns_key_list_pop_key(ldns_key_list *key_list);

/** 
 * converts a ldns_key to a public key rr
 *
 * \param[in] k the ldns_key to convert
 * \return ldns_rr representation of the key
 */
ldns_rr *ldns_key2rr(const ldns_key *k);

/**
 * print a private key to the file ouput
 * 
 * \param[in] output the FILE descriptor where to print to
 * \param[in] k the ldns_key to print
 */
void ldns_key_print(FILE *output, const ldns_key *k);

/**
 * frees a key structure
 *
 * \param[in] key the key object to free
 */
void ldns_key_free(ldns_key *key);

/**
 * frees a key structure and all it's internal data
 *
 * \param[in] key the key object to free
 */
void ldns_key_deep_free(ldns_key *key);

/**
 * Frees a key list structure
 * \param[in] key_list the key list object to free
 */
void ldns_key_list_free(ldns_key_list *key_list);

#endif /* LDNS_KEYS_H */
