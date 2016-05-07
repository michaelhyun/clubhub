# This file should contain all the record creation needed to seed the database with its default values.
# The data can then be loaded with the rake db:seed (or created alongside the db with db:setup).
#
# Examples:
#
#   cities = City.create([{ name: 'Chicago' }, { name: 'Copenhagen' }])
#   Mayor.create(name: 'Emanuel', city: cities.first)
Category.create({genre: 'Academics'})
Category.create({genre: 'Engineering'})
Category.create({genre: 'Cultural'})
Category.create({genre: 'Religious'})
Category.create({genre: 'Science'})
Category.create({genre: 'Other'})
Category.create({genre: 'Service and Social Justice'})

Category.create({genre: 'Performing Arts'})
Category.create({genre: 'Greek'})
Category.create({genre: 'Arts'})
Category.create({genre: 'Health'})
Category.create({genre: 'Music'})
Category.create({genre: 'Politics'})
Category.create({genre: 'Sports'})

#Club.create(Name: 'Tennis', Admin: 'Kaanchana', NumberOfMembers: '3', category: 'Sports')
#Club.create(Name: 'Alpha Xi Delta', Admin: 'Joanna', NumberOfMembers: '3', ategory: 'Greek')
#Club.create(Name: 'Premed', Admin: 'Jashan', NumberOfMembers: '3', Category: 'Health')
#Club.create(Name: 'Chemistry', Admin: 'michael', NumberOfMembers: '3', Category: 'Science')
#Club.create(Name: 'Drama', Admin: 'Alex', NumberOfMembers: '3', category: 'Performing Arts')
