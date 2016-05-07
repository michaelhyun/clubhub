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

#Club.create({name: 'Tennis'}, {admin: 'Kaanchana'}, {numberOfMembers: '3'}, {category: 'Sports'})
#Club.create({name: 'Alpha Xi Delta'}, {admin: 'Joanna'}, {numberOfMembers: '3'}, {category: 'Greek'})
#Club.create({name: 'Premed'}, {admin: 'Jashan'}, {numberOfMembers: '3'}, {category: 'Health'})
#Club.create({name: 'Chemistry'}, {admin: 'michael'}, {numberOfMembers: '3'}, {category: 'Science'})
#Club.create({name: 'Drama'}, {admin: 'Alex'}, {numberOfMembers: '3'}, {category: 'Performing Arts'})
