/*
 *  Copyright (C) 2004, Jon Gettler
 *  http://mvpmc.sourceforge.net/
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ident "$Id$"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <sys/ioctl.h>

#include "mvp_osd.h"
#include "osd.h"
#include "surface.h"

#if 0
#define PRINTF(x...) printf(x)
#else
#define PRINTF(x...)
#endif

static osd_surface_t *all[128];

static int full_width = 720, full_height = 480;

void
osd_set_surface_size(int w, int h)
{
	full_width = w;
	full_height = h;
}

int
osd_get_surface_size(osd_surface_t *surface, int *w, int *h)
{
	if (surface == NULL)
		return -1;

	if (w)
		*w = surface->sfc.width;
	if (h)
		*h = surface->sfc.height;

	return 0;
}

/*
 * osd_create_surface() - create a drawing surface
 */
osd_surface_t*
osd_create_surface(int w, int h)
{
	osd_surface_t *surface;
	int ret, num = 0;
	int i;

	if (w == -1)
		w = full_width;
	if (h == -1)
		h = full_height;

	PRINTF("%s(): stbgfx %d\n", __FUNCTION__, stbgfx);

	if (stbgfx < 0) {
		if ((stbgfx=open("/dev/stbgfx", O_RDWR)) < 0)
			return NULL;
		gfx_init();
	}

	PRINTF("%s(): stbgfx %d\n", __FUNCTION__, stbgfx);

	if ((surface=malloc(sizeof(*surface))) == NULL)
		return NULL;
	memset(surface, 0, sizeof(*surface));

	do {
		ret = ioctl(stbgfx, GFX_FB_SET_OSD, &num);
	} while ((ret != 0) && (num++ < 16));

	if (ret != 0)
		goto err;

	if (ioctl(stbgfx, GFX_FB_OSD_SURFACE, &num) != 0)
		goto err;

	memset(&surface->sfc, 0, sizeof(surface->sfc));
	surface->sfc.width = w;
	surface->sfc.height = h;
	surface->sfc.flags = 0x3f1533;
	surface->sfc.unknown = 1;
	if (ioctl(stbgfx, GFX_FB_SFC_ALLOC, &surface->sfc) != 0)
		goto err;

	memset(&surface->map, 0, sizeof(surface->map));
	surface->map.map[0].unknown = surface->sfc.handle;
	if (ioctl(stbgfx, GFX_FB_MAP, &surface->map) != 0)
		goto err;

	if ((surface->base[0]=mmap(NULL, surface->map.map[0].size,
				   PROT_READ|PROT_WRITE, MAP_SHARED, stbgfx,
				   surface->map.map[0].addr)) == MAP_FAILED)
		goto err;
	if ((surface->base[1]=mmap(NULL, surface->map.map[1].size,
				   PROT_READ|PROT_WRITE, MAP_SHARED, stbgfx,
				   surface->map.map[1].addr)) == MAP_FAILED)
		goto err;
	if ((surface->base[2]=mmap(NULL, surface->map.map[2].size,
				   PROT_READ|PROT_WRITE, MAP_SHARED, stbgfx,
				   surface->map.map[2].addr)) == MAP_FAILED)
		goto err;

	surface->display.num = num;
	if ((ret=ioctl(stbgfx, GFX_FB_MOVE_DISPLAY, &surface->display)) != 0)
		goto err;
	PRINTF("Display width: %ld  height: %ld\n",
	       surface->display.width, surface->display.height);

	if ((ret=ioctl(stbgfx, GFX_FB_SET_DISPLAY, &surface->display)) != 0)
		goto err;

	PRINTF("surface 0x%.8x created of size %d x %d   [%d]\n",
	       surface, w, h, surface->map.map[0].size);

	i = 0;
	while ((all[i] != NULL) && (i < 128))
		i++;
	if (i < 128)
		all[i] = surface;

	return surface;

 err:
	if (surface)
		free(surface);

	return NULL;
}

int
osd_destroy_surface(osd_surface_t *surface)
{
	int i;

	i = 0;
	while ((all[i] != surface) && (i < 128))
		i++;
	if (i < 128)
		all[i] = NULL;

	for (i=0; i<3; i++)
		if (surface->base[i])
			munmap(surface->base[i], surface->map.map[i].size);

	if (ioctl(stbgfx, GFX_FB_SFC_FREE, &surface->sfc) != 0)
		return -1;

	free(surface);

	PRINTF("surface destroyed\n");

	return 0;
}

void
osd_display_surface(osd_surface_t *surface)
{
	unsigned long fb_descriptor[2];

	fb_descriptor[0] = surface->sfc.handle;
	fb_descriptor[1] = 1;
	
	ioctl(stbgfx, GFX_FB_ATTACH, fb_descriptor);
}

void
osd_destroy_all_surfaces(void)
{
	int i;

	for (i=0; i<128; i++) {
		if (all[i])
			osd_destroy_surface(all[i]);
	}
}