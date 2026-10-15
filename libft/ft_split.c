/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_split.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 15:11:58 by nachebbi          #+#    #+#             */
/*   Updated: 2025/10/06 02:10:39 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	word_counter(const char *s, char c)
{
	int	count;
	int	sep;

	count = 0;
	sep = 0;
	while (*s)
	{
		if (*s != c && sep == 0)
		{
			sep = 1;
			count++;
		}
		else if (*s == c)
			sep = 0;
		s++;
	}
	return (count);
}

static char	*word_dup(const char *s, int start, int end)
{
	char	*word;
	int		i;

	i = 0;
	word = malloc((end - start + 1) * sizeof(char));
	if (!word)
		return (NULL);
	while (start < end)
		word[i++] = s[start++];
	word[i] = '\0';
	return (word);
}

char	**ft_split(char const *s, char c)
{
	int		i;
	int		j;
	int		index;
	char	**res;

	res = malloc((word_counter(s, c) + 1) * sizeof(char *));
	if (!res || !s)
		return (0);
	i = 0;
	j = 0;
	index = -1;
	while (i <= (int)ft_strlen(s))
	{
		if (s[i] != c && index < 0)
			index = i;
		else if ((s[i] == c || i == (int)ft_strlen(s)) && index >= 0)
		{
			res[j++] = word_dup(s, index, i);
			index = -1;
		}
		i++;
	}
	res[j] = 0;
	return (res);
}
