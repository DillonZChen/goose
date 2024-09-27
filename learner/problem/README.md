Originally I planned to do everything in unified_planning. However, 
1. it's super slow at grounding, some problems take >hours while NFD takes a second
2. NFD is the backend search so juggling APIs is annoying
3. it introduces a lot of unwanted side effects when parsing problems
