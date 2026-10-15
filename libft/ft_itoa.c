/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_itoa.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@42lausanne.ch>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/05 15:58:40 by nachebbi          #+#    #+#             */
/*   Updated: 2025/10/05 16:41:25 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

static int	ft_intlen(int n)
{
	long	nb;
	int		i;

	nb = n;
	i = 1;
	if (nb < 0)
	{
		i++;
		nb = -nb;
	}
	while (nb / 10 >= 1)
	{
		nb /= 10;
		i++;
	}
	return (i);
}

char	*ft_itoa(int n)
{
	char	*str;
	int		len;
	int		offset;

	offset = 0;
	len = ft_intlen(n);
	str = (char *)malloc(sizeof(char) * len + 1);
	if (!str)
		return (NULL);
	str[len] = '\0';
	if (n < 0)
	{
		str[0] = '-';
		str[--len] = -(n % 10) + '0';
		n = (n / 10) * -1;
		offset = 1;
	}
	while (--len >= offset)
	{
		str[len] = n % 10 + '0';
		n /= 10;
	}
	return (str);
}
