// @ts-check
import { defineConfig } from "astro/config";
import starlight from "@astrojs/starlight";

// https://astro.build/config
export default defineConfig({
	site: "https://gargantuan.teamfireworks.org",
	integrations: [
		starlight({
			title: "Gargantuan",
			social: [
				{
					icon: "github",
					label: "GitHub",
					href: "https://github.com/teamfireworks/gargantuan",
				},
			],
			sidebar: [
				{
					label: "Guides",
					items: [
						{
							label: "Roblox Deviations",
							slug: "guides/roblox-deviations",
						},
					],
				},
				{
					label: "Developing",
					items: [
						{
							label: "Contributing to Gargantuan",
							slug: "developing/contributing-to-gargantuan",
						},
						{
							label: "Working on Gargantuan",
							slug: "developing/working-on-gargantuan",
						},
						{
							label: "Roadmap",
							slug: "developing/roadmap",
						},
						{
							label: "Code of Conduct",
							slug: "developing/code-of-conduct",
						},
					],
				},
				{
					label: "Meta",
					items: [
						{
							label: "License",
							slug: "meta/license",
						},
						{
							label: "Additional Copyright Notices",
							slug: "meta/additional-copyright-notices",
						},
					],
				},
			],
		}),
	],
});
