/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_strdup.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nachebbi <nachebbi@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/10/04 15:50:36 by nachebbi          #+#    #+#             */
/*   Updated: 2025/10/04 15:58:38 by nachebbi         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

char	*ft_strdup(const char *s)
{
	char	*ret;

	ret = malloc(sizeof (char) * ft_strlen(s) + 1);
	if (!ret)
		return (0);
	ft_strlcpy(ret, s, ft_strlen(s) + 1);
	return (ret);
}
