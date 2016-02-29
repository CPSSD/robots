Acceptance criteria are a a crucial aspect of agile development that I feel we have neglected on this project to date.

Acceptance criteria are a strict set of criteria that defines when an issue is considered completed. They should (ideally) have a bijective relationship with the issues of a project. As a project gets increasingly technical or difficult to define acceptance tests become even more crucial as they allow you to categorically say what size an issue is, how much time/effort should be allocated to the issue and most importantly when the issue is considered ready for review/complete. As unit tests are largely ineffective for this project I feel like we should start using acceptance tests as soon as possible.

Acceptance criteria are usually of the format:
  "As a \<user/admin/owner/..\> I want to \<be able to log in/be able to view a map of an area on the website/..\> when I \<input the correct username and password on the login page/run the mapping program on the Edison/..\>."
  
They are usually of the format "As \<someone interacting with the project\> I want to \<well defined result\> when I \<do an action\>."

I think by default all issues should have acceptance criteria agreed upon during the planning stages of the sprint unless it is agreed that an acceptance test doesn't suit the issue. I also think that these acceptance criteria should be noted on the matching issue in Github.
