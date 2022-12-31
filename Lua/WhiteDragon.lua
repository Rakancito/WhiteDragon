quest WhiteDragon begin
	state start begin
		when 20815.chat."Mazmorra Alastor (Fácil)" begin
			say_title(mob_name(20815))
			say("¿Deseas intentar derrotar a Alastor?")

			local s= select("Si", "No")
			if s == 2 then
				return
			end

			if party.is_party() then
				if not party.is_leader() then
					say("Debes ser líder de un grupo.")
					return
				end

				local pids = {party.get_member_pids()}

				local timerCheck = true
				local levelCheck = true
				local ticketCheck = true

				for i, pid in next, pids, nil do
					q.begin_other_pc_block(pid)

					if ((get_global_time() - pc.getf("white_dragon","last_exit_white_dragon")) < 60*60*3) then 
						timerCheck = false
					end

					if pc.get_conqueror_level() < 20 then
						levelCheck = false
					end

					if pc.count_item(70430) < 1 then
						ticketCheck = false
					end

					q.end_other_pc_block()
				end


				if not timerCheck then
					say("Deben esperar 3 horas para reingresar.")
					return
				end

				if not levelCheck then
					say("Deben ser mínimo LvL.20 de Campeón.")
					return
				end

				if not ticketCheck then
					say("Para acceder, todos los miembros deben")
					say("tener: "..item_name(70430)..".")
            
					return
				end
                
				-->Quitar llaves
				for i, pid in next, pids, nil do
					q.begin_other_pc_block(pid)
					pc.remove_item(70430, 1)
					pc.setf("white_dragon","last_exit_white_dragon", get_global_time())
					q.end_other_pc_block()
				end
			else
				if pc.get_conqueror_level() < 20 then
					say("Debes ser mínimo LvL.20 de Campeón.")
					return
				end

				if ((get_global_time() - pc.getf("white_dragon","last_exit_white_dragon")) < 60*60*3) then 
					say("Debes esperar 3 horas para reingresar.")
					return
				end

				if pc.count_item(70430) < 1 then
					say("Debes tener al menos un pasaje de entrada en tu inventario.")
					return
				end

				pc.setf("white_dragon","last_exit_white_dragon", get_global_time())
				pc.remove_item(70430, 1)
			end

			onlychat_notice_all("[CH "..get_channel_id().."]: ¡El grupo de "..pc.get_name().." ha ingresado a la Mazmorra de Alastor!")
			WhiteLair.Access()

		end

		when 4030.chat."Comenzar la Mazmorra" begin
			say_title(mob_name(4030))
			say("¿Deseas comenzar la mazmorra?")
			local s=select("Si", "No")

			if s == 1 then
				WhiteLair.StartDungeon()
			end
		end

	end
end
