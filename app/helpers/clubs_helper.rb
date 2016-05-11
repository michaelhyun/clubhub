module ClubsHelper
	def ratyrate_rateable
	end
	def checked
        list = [Sports]
        return list
    end


    def print(category)
     begin
        db = SQLite3::Database.open "development.db"
        
        #goes through each element of the array and then inputs that into the query
        
        stm = db.execute "SELECT name FROM clubs WHERE category = category"


            stm.each do |row|
                puts "%s\n", row['id']

            end

        rescue SQLite3::Exception => e 

            puts "Exception occurred"
            puts e
            

        ensure
            db.close if db
        end
    end
end