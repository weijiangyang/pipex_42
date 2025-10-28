/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   get_next_line_utils.c                              :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: weiyang <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/24 14:42:25 by weiyang           #+#    #+#             */
/*   Updated: 2025/05/28 09:57:55 by weiyang          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "get_next_line.h"

int	end_line(char *str)
{
	while (*str)
	{
		if (*str == '\n')
			return (1);
		str++;
	}
	return (0);
}

char	*ft_strdup(char *s)
{
	char	*ptr;
	char	*dst;
	int		i;

	i = 0;
	if (!s)
		return (NULL);
	ptr = malloc ((ft_strlen(s) + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	dst = ptr;
	while (s[i])
	{
		dst[i] = s[i];
		i++;
	}
	dst[i] = '\0';
	return (ptr);
}

int	ft_strlen(char *str)
{
	int	size;

	size = 0;
	while (*str)
	{
		size++;
		str++;
	}
	return (size);
}

char	*extract_line(char *str)
{
	int		i;
	char	*extract;
	int		j;

	j = 0;
	i = 0;
	if (!str || str[0] == '\0')
		return (NULL);
	while (str[i] && str[i] != '\n')
		i++;
	if (str[i] == '\n')
		i++;
	extract = (char *) malloc ((i + 1) * sizeof (char));
	if (!extract)
		return (NULL);
	while (j < i)
	{
		extract[j] = str[j];
		j++;
	}
	extract[j] = '\0';
	return (extract);
}

char	*ft_strjoin(char *s1, char *s2)
{
	char	*ptr;
	char	*dst;

	if (!s1)
		s1 = "";
	if (!s2)
		s2 = "";
	ptr = malloc ((ft_strlen(s1) + ft_strlen(s2) + 1) * sizeof(char));
	if (!ptr)
		return (NULL);
	dst = ptr;
	while (*s1)
	{
		*dst = *s1;
		dst++;
		s1++;
	}
	while (*s2)
	{
		*dst = *s2;
		dst++;
		s2++;
	}
	*dst = '\0';
	return (ptr);
}
