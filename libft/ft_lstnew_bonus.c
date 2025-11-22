/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstnew_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 09:15:41 by joleksia          #+#    #+#             */
/*   Updated: 2025/05/08 09:19:48 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstnew(void *content)
{
	t_list	*_list;

	_list = ft_calloc(1, sizeof(t_list));
	if (!_list)
		return (0);
	_list->content = content;
	_list->next = NULL;
	return (_list);
}
