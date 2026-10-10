/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_memchr.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/09/28 21:26:41 by nachebbi          #+#    #+#             */
/*   Updated: 2025/10/06 01:03:40 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <string.h>

void	*ft_memchr(char const *s, int c, size_t n)
{
	size_t			i;
	unsigned char	*str;
	unsigned char	clue;

	i = 0;
	str = (unsigned char *)s;
	clue = (unsigned char)c;
	while (i < n)
	{
		if (str[i] == clue)
			return ((void *)&str[i]);
		i++;
	}
	return (NULL);
}
