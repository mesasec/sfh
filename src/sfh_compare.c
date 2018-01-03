#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <assert.h>
#include <unistd.h>
#include "sfh_internal.h"

#define EDIT_DISTN_INSERT_COST 1
#define EDIT_DISTN_REMOVE_COST 1
#define EDIT_DISTN_REPLACE_COST 2
#ifndef MAX
#define MAX(a, b)  	(((a) > (b)) ? (a) : (b))
#endif

#ifndef MIN
#define MIN(a, b)  	(((a) < (b)) ? (a) : (b))
#endif


int GIE_cmp(const void * a, const void * b)
{
	unsigned int tmp_a = *(unsigned int *)a;
	unsigned int tmp_b = *(unsigned int *)b;
	if(before(tmp_a, tmp_b))
	{
		return -1;
	}
	else if(after(tmp_a, tmp_b))
	{
		return 1;
	}	
	else
	{
		return 0;
	}
}

inline unsigned long long calc_fh_blocksize(unsigned long long orilen)
{
	double tmp = orilen/(64 * BLOCKSIZE_MIN);
	double index = floor(log(tmp)/log(2));
	double tmp_t = pow(2,index);
	unsigned long long blocksize = (unsigned long long)(tmp_t * BLOCKSIZE_MIN);
	return blocksize;
}

inline unsigned long long get_blocksize_from_head(const char * fuzzy_string, unsigned int str_len)
{
	const char * tmp_str = fuzzy_string;
	char blk[100];
	memset(blk,'\0',sizeof(blk));
	unsigned long long blocksize = 0;
	int i = 0;
	while(*tmp_str != '\0' && *tmp_str != ':' && str_len != 0 && i < 100)
	{
		blk[i++] = *tmp_str;
		tmp_str++;
		str_len--;
	}
	blocksize = (unsigned long long)atoi(blk);
	return blocksize;
}
inline unsigned int get_real_length(const char * string, unsigned int length)
{
	unsigned int ret = 0;
	const char * tmp_str = string;
	while(*tmp_str != '\0')
	{
		if(*tmp_str == '[')
		{
			break;
		}
		tmp_str++;
		ret ++;
	}
	return ret; 
}

int edit_distn(const char *s1, int s1len, const char *s2, int s2len) 
{
  long int max_len = 0;
  if(s1len >= s2len)
  {  
         max_len = s1len;
  }
  else
  {
         max_len = s2len;
  }
  int **t = (int **)malloc(2*sizeof(int *));
  t[0] = (int *)malloc((max_len +1)*sizeof(int));
  t[1] = (int *)malloc((max_len +1)*sizeof(int));
  //int t[2][EDIT_DISTN_MAXLEN+1];
  int *t1 = t[0];
  int *t2 = t[1];
  int *t3;
  size_t i1, i2;
  for (i2 = 0; i2 <= s2len; i2++)
    t[0][i2] = i2 * EDIT_DISTN_REMOVE_COST;
  for (i1 = 0; i1 < s1len; i1++) {
    t2[0] = (i1 + 1) * EDIT_DISTN_INSERT_COST;
    for (i2 = 0; i2 < s2len; i2++) {
      int cost_a = t1[i2+1] + EDIT_DISTN_INSERT_COST;
      int cost_d = t2[i2] + EDIT_DISTN_REMOVE_COST;
      int cost_r = t1[i2] + (s1[i1] == s2[i2] ? 0 : EDIT_DISTN_REPLACE_COST);
      t2[i2+1] = MIN(MIN(cost_a, cost_d), cost_r);
    }
    t3 = t1;
    t1 = t2;
    t2 = t3;
  }
  long int ret = t1[s2len];
  free(t[0]);
  free(t[1]);
  free(t);
  return ret;
  //return t1[s2len];
}

int GIE_string_similiarity(const char *str1, int len1, const char *str2, int len2)
{
	int edit_distance=0;
	int conf=0;
	edit_distance = edit_distn(str1, len1,str2,len2);
	conf = 100-(edit_distance*100)/(len1 + len2);
	return conf;
}

int SFH_similiarity(const char *sfh1, int len1, const char *sfh2, int len2)
{
	int j = 0, t = 0;
	unsigned long long query_blocksize = 0, index_blocksize = 0;
	unsigned int query_real_length = 0, index_real_length = 0;
	const char *query_gram_begin = sfh1;
	const char *index_gram_begin = sfh2;
	char *splice_str = (char *)malloc(sizeof(char)*len1);
	memset(splice_str,'\0',len1);
	char *spli_str_begin = splice_str;
	int edit_distance = 0;
	int ret = 0;
	char *p = NULL;
	int splice_len = 0;
	
	for(j = 0; j < 2; j++)
	{				
		index_blocksize = get_blocksize_from_head(index_gram_begin, len2);
		while((*index_gram_begin) != '\0')
		{
			if((*index_gram_begin) == ':')
			{
				index_gram_begin++;
				break;
			}
			index_gram_begin++;
		}
		index_real_length = get_real_length(index_gram_begin, len2);
		query_gram_begin = sfh1;
		for(t = 0; t < 2; t++)
		{
				query_blocksize = get_blocksize_from_head(query_gram_begin, len1);
				//printf("gram_begin:%c\n",*index_gram_begin);
				//printf("gram_str:%s\n",index_gram_begin);
				while((*query_gram_begin) != '\0')
				{
					if((*query_gram_begin) == ':')
					{
						query_gram_begin++;
						break;
					}
					query_gram_begin++;
				}
				//printf("query_blocksize:%lld, index_blocksize:%lld\n",query_blocksize,index_blocksize);
				//index_real_length = get_real_length(index_gram_begin, len1);
				if(query_blocksize == index_blocksize)
				{
					while((*query_gram_begin) != '#' && (*query_gram_begin) != '\0')
					{		
						p=strchr(query_gram_begin,'[');
						if(p!=NULL)
						{
							query_real_length = p-query_gram_begin;
							p=strchr(p,']');
							if(p != NULL && (*p) != '\0')
							{
								
								memcpy(spli_str_begin,query_gram_begin,query_real_length);
								spli_str_begin += query_real_length;
						    //edit_distance += edit_distn(query_gram_begin, query_real_length, index_gram_begin, index_real_length);
								query_gram_begin = p+1;
							}
							else
							{
								break;
							}
						}
						else
						{
							break;
						}
					}				
					splice_len = strnlen(splice_str,len1);
					edit_distance = edit_distn(index_gram_begin, index_real_length, splice_str, splice_len);
					//printf("query_real_length:%d splice_length:%d edit_distance:%d\n",query_real_length,splice_len,edit_distance);
					ret = 100-(edit_distance*100)/(index_real_length + splice_len);
					//ret = (100*ret)/SPAM_LENGTH;
					//ret = 100-ret;
					//ret = 100 - (100*edit_distance)/(query_real_length);
					free(splice_str);
					return ret;
				}
				while(*query_gram_begin != '\0')
				{
					if(*query_gram_begin == '#')
					{
						query_gram_begin++;
						break;
					}
					query_gram_begin++;
				}				
			
		}
		while(*index_gram_begin != '\0')
		{
			if(*index_gram_begin == '#')
			{
				index_gram_begin++;
				break;
			}
			index_gram_begin++;
		}				
	}
	//printf("no blocksize:query_real_length:%d splice_length:%d edit_distance:%d\n",query_real_length,splice_len,edit_distance);
	free(splice_str);
	return 0;
}



