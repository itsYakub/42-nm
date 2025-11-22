/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ft_lstmap_bonus.c                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: joleksia <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/05/08 09:15:41 by joleksia          #+#    #+#             */
/*   Updated: 2025/06/30 11:35:40 by joleksia         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "libft.h"

t_list	*ft_lstmap(t_list *lst, void *(*f)(void *), void (*del)(void *))
{
	t_list	*_node;
	t_list	*_new;

	if (!lst)
		return (0);
	_new = 0;
	while (lst)
	{
		_node = ft_lstnew(f(lst->content));
		if (!_node)
		{
			ft_lstclear(&_node, del);
			return (0);
		}
		ft_lstadd_back(&_new, _node);
		lst = lst->next;
	}
	return (_new);
}
